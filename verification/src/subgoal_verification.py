import subprocess
import tempfile
import shutil
import re
import sys
from pathlib import Path
from typing import Any


class GoogleBenchmarkVerifier():
    """
    Performance verifier using Google Benchmark.
    """
    def __init__(
        self, test_sizes: list[int], isa: str = 'AVX',
        template_path: str | Path | None = None,
    ):
        # fill google benchmark template + variables
        self.test_sizes: list[int] = test_sizes

        # set default template path relative to this file's location
        if template_path is None:
            resolved_template_path = Path(__file__).parent / 'templates' / 'google_benchmark_template.txt'
        else:
            resolved_template_path = Path(template_path)

        self.template_path: Path = resolved_template_path
        self.template: str = self.load_template(self.template_path)

        # compiler variables
        self.compiler: str = 'g++'
        self.isa_flags: dict[str, list[str]] = {
            'AVX': ['-mavx', '-mavx2', '-mfma'],
            'SSE': ['-msse', '-msse2', '-msse4.1'],
            'Neon': ['-march=armv8-a+simd'],
        }

        self.isa: str = isa

        # Detect benchmark library paths
        self.benchmark_include:str
        self.benchmark_lib:str
        self.benchmark_include, self.benchmark_lib = self._detect_benchmark_paths()

    def _detect_benchmark_paths(self) -> tuple[str, str]:
        """
        Detect Google Benchmark include and library paths.
        Tries Homebrew first, then common system paths.
        """
        # Try Homebrew on macOS
        if sys.platform == 'darwin':
            try:
                result = subprocess.run(
                    ['brew', '--prefix', 'google-benchmark'],
                    capture_output=True,
                    text=True,
                    check=True,
                    timeout=5
                )
                brew_prefix = Path(result.stdout.strip())
                include_path = brew_prefix / 'include'
                lib_path = brew_prefix / 'lib'

                if include_path.exists() and lib_path.exists():
                    return str(include_path), str(lib_path)
            except (subprocess.CalledProcessError, subprocess.TimeoutExpired, FileNotFoundError):
                pass

        # Fallback to common system paths
        common_paths = [
            ('/opt/homebrew/include', '/opt/homebrew/lib'),  # Apple Silicon
            ('/usr/local/include', '/usr/local/lib'),        # Intel Mac / Linux
            ('/usr/include', '/usr/lib'),                     # Linux system
        ]

        for inc, lib in common_paths:
            inc_path = Path(inc)
            lib_path = Path(lib)
            if (inc_path / 'benchmark').exists() or inc_path.exists():
                return inc, lib

        # If nothing found, return empty strings (will likely fail at compile time)
        return '', ''

    def load_template(self, template_path: Path) -> str:
        """Load benchmark template from file."""
        try:
            return template_path.read_text(encoding='utf-8')
        except FileNotFoundError as e:
            raise FileNotFoundError(
                f"Template file not found: {template_path}\n" +
                f"Expected location: {template_path.absolute()}"
            ) from e
        except Exception as e:
            raise RuntimeError(f"Error reading template file: {e}") from e

    def fill_template(self, scalar_code: str, simd_code: str, template: str, test_sizes: list[int]) -> str:
        """Fill benchmark template with scalar and SIMD code."""
        filled = template.replace('{{SCALAR_CODE}}', scalar_code)
        filled = filled.replace('{{SIMD_CODE}}', simd_code)
        sizes_str = ', '.join(str(s) for s in test_sizes)
        filled = filled.replace('{{TEST_SIZES}}', sizes_str)
        return filled

    def verify(self, scalar_code: str, simd_code: str):
        """
        Verify SIMD implementation against scalar baseline.

        Args:
            scalar_code: C++ scalar implementation
            simd_code: C++ SIMD implementation

        Returns:
            Dictionary with verification results including speedups or error info
        """
        benchmark_code = self.fill_template(scalar_code, simd_code, self.template, self.test_sizes)

        # Compile benchmark
        compile_result = self.compile_benchmark(benchmark_code)
        if not compile_result['success']:
            self._cleanup(compile_result['tmpdir'])
            return {
                'success': False,
                'stage': 'compilation',
                'error': compile_result['error']
            }

        # Run benchmark
        run_result = self.run_benchmark(compile_result['binary_path'])
        if not run_result['success']:
            self._cleanup(compile_result['tmpdir'])
            return {
                'success': False,
                'stage': 'execution',
                'error': run_result['error']
            }

        # Parse results
        try:
            speedups = self.parse_benchmark_output(run_result['output'])
            self._cleanup(compile_result['tmpdir'])

            return {
                'success': True,
                'speedups': speedups,
                'raw_output': run_result['output']
            }
        except Exception as e:
            self._cleanup(compile_result['tmpdir'])
            return {
                'success': False,
                'stage': 'parsing',
                'error': str(e)
            }

    def compile_benchmark(self, source_code: str) -> dict[str, Any]:
        """Compile benchmark source code to executable."""
        tmp_dir = Path(tempfile.mkdtemp())
        src_path = tmp_dir / 'benchmark.cpp'
        bin_path = tmp_dir / 'benchmark'

        try:
            src_path.write_text(source_code, encoding='utf-8')

            cmd = [
                self.compiler,
                '-std=c++17',
                '-O3',
            ]

            # Add include and lib paths if detected
            if self.benchmark_include:
                cmd.append(f'-I{self.benchmark_include}')
            if self.benchmark_lib:
                cmd.append(f'-L{self.benchmark_lib}')

            # Add ISA-specific flags
            cmd += self.isa_flags.get(self.isa, [])

            # Add source, output, and libraries
            cmd += [
                str(src_path),
                '-o', str(bin_path),
                '-lbenchmark',
                '-lpthread'
            ]

            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=30
            )

            if result.returncode != 0:
                return {
                    'success': False,
                    'error': result.stderr,
                    'tmpdir': str(tmp_dir)
                }

            return {
                'success': True,
                'binary_path': str(bin_path),
                'tmpdir': str(tmp_dir)
            }

        except subprocess.TimeoutExpired:
            return {
                'success': False,
                'error': 'Compilation timeout (>30s)',
                'tmpdir': str(tmp_dir)
            }
        except Exception as e:
            return {
                'success': False,
                'error': str(e),
                'tmpdir': str(tmp_dir)
            }

    def run_benchmark(self, binary_path: str) -> dict[str, Any]:
        """Run compiled benchmark binary and capture output."""
        try:
            result = subprocess.run(
                [binary_path, '--benchmark_format=console'],
                capture_output=True,
                text=True,
                timeout=30
            )

            if result.returncode != 0:
                return {
                    'success': False,
                    'error': result.stderr
                }

            return {
                'success': True,
                'output': result.stdout
            }

        except subprocess.TimeoutExpired:
            return {
                'success': False,
                'error': 'Benchmark timeout (>30s)'
            }
        except Exception as e:
            return {
                'success': False,
                'error': str(e)
            }

    def parse_benchmark_output(self, output: str) -> dict[int, float]:
        """
        Parse benchmark output and calculate speedups.

        Args:
            output: Raw benchmark console output

        Returns:
            Dictionary mapping test size to speedup factor
        """
        scalar_times: dict[int, float] = {}
        simd_times: dict[int, float] = {}
        pattern = r'(\w+)/(\d+)\s+(\d+)\s+ns'

        for line in output.split('\n'):
            match = re.search(pattern, line)
            if match:
                name, size, time_ns = match.groups()
                size = int(size)
                time_ns = float(time_ns)

                if name == 'Scalar':
                    scalar_times[size] = time_ns
                elif name == 'SIMD':
                    simd_times[size] = time_ns

        # Calculate speedups
        speedups: dict[int, float] = {}
        for size in scalar_times:
            if size in simd_times and simd_times[size] > 0:
                speedup = scalar_times[size] / simd_times[size]
                speedups[size] = speedup

        return speedups

    def _cleanup(self, tmpdir: str) -> None:
        """Clean up temporary directory safely."""
        if not tmpdir:
            return

        tmp_path = Path(tmpdir)
        if tmp_path.exists() and tmp_path.is_dir():
            try:
                shutil.rmtree(tmp_path, ignore_errors=True)
            except Exception:
                pass  # Ignore cleanup errors

class DifferentialVerifier():
    def __init__(self):
        pass

class CompilationVerifier():
    def __init__(self):
        pass

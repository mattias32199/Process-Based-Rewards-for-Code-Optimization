# scripts/test_benchmark.py
import subprocess
import tempfile
import os

def test_google_benchmark():
    """Test if Google Benchmark is properly installed"""

    test_code = """
#include <benchmark/benchmark.h>

static void BM_Test(benchmark::State& state) {
    for (auto _ : state) {
        int sum = 0;
        for (int i = 0; i < 1000; i++) {
            sum += i;
        }
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_Test);
BENCHMARK_MAIN();
"""

    with tempfile.TemporaryDirectory() as tmpdir:
        # Write code
        src_path = os.path.join(tmpdir, 'test.cpp')
        bin_path = os.path.join(tmpdir, 'test')

        with open(src_path, 'w') as f:
            f.write(test_code)

        # Compile
        # Get Homebrew prefix for google-benchmark
        import subprocess as sp
        try:
            brew_prefix = sp.run(
                ['brew', '--prefix', 'google-benchmark'],
                capture_output=True,
                text=True,
                check=True
            ).stdout.strip()
        except:
            brew_prefix = '/opt/homebrew/opt/google-benchmark'  # fallback
        
        compile_cmd = [
            'g++', '-std=c++17', '-O3',
            f'-I{brew_prefix}/include',
            f'-L{brew_prefix}/lib',
            src_path, '-o', bin_path,
            '-lbenchmark', '-lpthread'
        ]

        compile_result = subprocess.run(
            compile_cmd,
            capture_output=True,
            text=True
        )

        if compile_result.returncode != 0:
            print("❌ Compilation failed!")
            print(compile_result.stderr)
            return False

        print("✅ Compilation successful!")

        # Run
        run_result = subprocess.run(
            [bin_path],
            capture_output=True,
            text=True,
            timeout=5
        )

        if run_result.returncode == 0:
            print("✅ Benchmark execution successful!")
            print(run_result.stdout)
            return True
        else:
            print("❌ Benchmark execution failed!")
            print(run_result.stderr)
            return False

if __name__ == "__main__":
    test_google_benchmark()

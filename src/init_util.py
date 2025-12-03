import os

# Default paths (can be moved to config later)
DEFAULT_INCLUDE_PATH = 'benchmark/include'
DEFAULT_LIB_PATH = 'benchmark/build/src/libbenchmark.a'

def check_google_benchmark(include_path=DEFAULT_INCLUDE_PATH, lib_path=DEFAULT_LIB_PATH):
    """
    Asserts that Google Benchmark dependencies exist.
    Raises FileNotFoundError if they are missing.
    """
    # 1. Check for the library file (.a)
    if not os.path.exists(lib_path):
        raise FileNotFoundError(
            f"[Verifier Error] Google Benchmark library not found at: {lib_path}\n"
            "Please compile Google Benchmark or check your paths."
        )

    header_file = os.path.join(include_path, 'benchmark', 'benchmark.h')
    if not os.path.exists(header_file):
        raise FileNotFoundError(
            f"[Verifier Error] Google Benchmark headers not found at: {header_file}\n"
            "Please check your include path."
        )

    print(f"[Verifier] Google Benchmark found at: {lib_path}")

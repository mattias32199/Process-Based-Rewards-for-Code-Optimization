# /src/correctness.py
import subprocess
import tempfile
import re
from pathlib import Path

from src.simdbench.data import stream_jsonl
from src.simdbench.global_var import *


ROOT = os.path.dirname(os.path.abspath(__file__))
benchmark_src = os.path.join(ROOT, "../", "benchmark/build/src")
benchmark_include = os.path.join(ROOT, "../", "benchmark/include")
utils = os.path.join(ROOT, "", "simdbench/utils")

def clean_generated_code(output_str: str, entry_point: str) -> str:
    # Regular expression to match Markdown code blocks with optional language tag
    # code_block_pattern = re.compile(r"```(?:\w*\n)?(.*?)```", re.DOTALL)
    code_block_pattern = re.compile(r"```[^\n]*\n(.*?)```", re.DOTALL)
    matches = code_block_pattern.finditer(output_str)
    res = output_str
    for match in matches:
        code = match.group(1)
        if entry_point in code:
            res =  code.strip()
            break 
    
    # Clean Unicode quotes and special characters
    replacements = {
        '\u2018': "'",  # ' -> '
        '\u2019': "'",  # ' -> '
        '\u201C': '"',  # " -> "
        '\u201D': '"',  # " -> "
        '\u2013': '-',  # – -> -
        '\u2014': '-',  # — -> -
    }
    for unicode_char, ascii_char in replacements.items():
        res = res.replace(unicode_char, ascii_char)
    
    return res

def clean_error_message(error_msg):
    """
    clean the path in the error message, keeping the filename.
    Filter out warnings and keep only error messages.
    example:
    input: "/home/XXX/main.cpp:15: error"
    output: "[REDACTED]/main.cpp:15: error"
    """
    # First, sanitize paths
    sanitized = re.sub(
        r'(?P<quote>["\'])?/home[\w\s./-]+/(?P<filename>[^/\s]+)(?P=quote)?',
        lambda m: (m.group("quote") or "") + "[REDACTED]/" + m.group("filename") + (m.group("quote") or ""),
        error_msg,
        flags=re.ASCII
    )
    sanitized = re.sub(
        r'(?P<quote>["\'])?/mnt[\w\s./-]+/(?P<filename>[^/\s]+)(?P=quote)?',
        lambda m: (m.group("quote") or "") + "[REDACTED]/" + m.group("filename") + (m.group("quote") or ""),
        sanitized,
        flags=re.ASCII
    )
    sanitized = re.sub(
        r'(?P<quote>["\'])?/jet[\w\s./-]+/(?P<filename>[^/\s]+)(?P=quote)?',
        lambda m: (m.group("quote") or "") + "[REDACTED]/" + m.group("filename") + (m.group("quote") or ""),
        sanitized,
        flags=re.ASCII
    )
    
    # Filter to keep only lines with "error:" (case-insensitive)
    lines = sanitized.split('\n')
    error_lines = [line for line in lines if re.search(r'\berror\b', line, re.IGNORECASE)]
    
    # If no error lines found, return the sanitized message (failsafe)
    if not error_lines:
        return sanitized
    
    return '\n'.join(error_lines)


def exec_cpp(cpp_code: str, intrinsic: str, timeout: float, googlebench: bool=False, optimization = "-O0", debug_file: str = None) -> tuple:
    '''
    0: something failed
    1: compilation and execution success
    debug_file: optional path to write code and error messages
    '''
    with tempfile.TemporaryDirectory(prefix='simdbench_', dir=ROOT) as tmp_dir:
        # These system calls are needed when cleaning up tempdir.
        import os
        import shutil
        rmtree = shutil.rmtree
        rmdir = os.rmdir
        chdir = os.chdir
        
        dt = tempfile.mktemp(dir='')
        # write cpp
        cpp_path = os.path.join(tmp_dir, f"{dt}.cpp")
        with open(cpp_path, "w", encoding="utf-8") as file:
            file.write(cpp_code)

        # compilation
        compilation_cmd = [
            cpp_compilers[intrinsic],  # e.g., "g++"
            cpp_path,                  # source file
        ]
        
        # Add intrinsic flags (filter out empty strings)
        intrinsic_flags = [flag for flag in intrin_flags[intrinsic].split() if flag]
        compilation_cmd.extend(intrinsic_flags)
        
        compilation_cmd.extend([
            "-std=c++17",
            f"-I{utils}",
            optimization
        ])
        #compilation_cmd = f"{cpp_compilers[intrinsic]} {cpp_path} {intrin_flags[intrinsic]} -std=c++17 -I{utils} "
        if(googlebench): 
            benchmark_flags = f"-L{benchmark_src} -isystem {benchmark_include} -lbenchmark -lpthread".split()
            compilation_cmd += benchmark_flags
        compilation_cmd = compilation_cmd + ["-o", os.path.join(tmp_dir, f'{dt}.out')]
        compilation_res = subprocess.run(compilation_cmd, shell=False, capture_output=True, text=True, timeout=timeout*0.25)
        if compilation_res.returncode != 0: # compilation failed
            error_msg = clean_error_message(compilation_res.stderr)
            if debug_file:
                with open(debug_file, 'a', encoding='utf-8') as f:
                    f.write("=" * 80 + "\n")
                    f.write("COMPILATION ERROR\n")
                    f.write("=" * 80 + "\n")
                    f.write("CODE:\n")
                    f.write(cpp_code + "\n\n")
                    f.write("ERROR:\n")
                    f.write(error_msg + "\n\n")
            return {
                'compiles': False, 
                'correct': False,
                'outcome': 'compilation_error',
                'feedback': f"compilation failed: {error_msg}"
            }
        
        # execution
        execution_cmd = [f"{os.path.join(tmp_dir, f'{dt}.out')}"]
        # Warning: emulators are not suitable for performance testing
        emulator = emulators[intrinsic] # None: no emulator needed
        if(emulator!=None): execution_cmd = [emulator] + execution_cmd
        if(googlebench): execution_cmd += ["--benchmark_format=json"]
        execution_res = subprocess.run(execution_cmd, shell=False, capture_output=True, text=True, timeout=timeout*0.7)
        if execution_res.returncode != 0: # runtime failed
            error_msg = clean_error_message(execution_res.stderr)
            if debug_file:
                with open(debug_file, 'a', encoding='utf-8') as f:
                    f.write("=" * 80 + "\n")
                    f.write("RUNTIME ERROR\n")
                    f.write("=" * 80 + "\n")
                    f.write("CODE:\n")
                    f.write(cpp_code + "\n\n")
                    f.write("ERROR:\n")
                    f.write(error_msg + "\n\n")
            return {
                'compiles': True, 
                'correct': False,
                'outcome': 'runtime_error',
                'feedback': f"runtime failed: {error_msg}"
            }
        
        # Needed for cleaning up.
        shutil.rmtree = rmtree
        os.rmdir = rmdir
        os.chdir = chdir
        
        return {
        'compiles': True,
        'correct': True,
        'outcome': 'pass',
        'feedback': f"execution succeeded, output: {execution_res.stdout}"
        }

def verify_simd_correctness(
    task_data: dict, 
    simd_solution: str,
    timeout: int = 30,
    debug_file: str = None
) -> dict: 
    code = (
        get_header() + '\n' + \
        task_data["solution_scalar"] + '\n' + \
        simd_solution + '\n' + \
        task_data["test_correctness"]
    )
    return exec_cpp(code, intrinsic=task_data.get("intrinsic", "AVX"), timeout=timeout, debug_file=debug_file)
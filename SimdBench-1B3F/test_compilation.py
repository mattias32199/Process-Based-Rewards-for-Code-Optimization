#!/usr/bin/env python3
"""
Test compilation of C++ code from SimdBench and TSVC datasets.
Verifies that test_correctness and test_performance code actually compiles.
"""

import json
import subprocess
import tempfile
import os
from pathlib import Path
import random
import sys


class CompilationTester:
    def __init__(self, utils_path):
        """
        Args:
            utils_path: Path to simdbench/utils directory containing Random.h, allclose.h
        """
        self.utils_path = Path(utils_path)
        self.results = {
            'correctness': {'passed': [], 'failed': []},
            'performance': {'passed': [], 'failed': []}
        }
    
    def compile_code(self, code, task_id, test_type, intrinsic='AVX'):
        """Try to compile C++ code and return success/failure."""
        
        # Map intrinsic to compiler flags
        intrinsic_flags = {
            'AVX': ['-mavx', '-mavx2'],
            'SSE': ['-msse', '-msse2', '-msse3', '-msse4.1', '-msse4.2'],
            'AVX512': ['-mavx512f'],
            'Neon': ['-march=armv8-a+simd'],
            'scalar': []
        }
        
        flags = intrinsic_flags.get(intrinsic, [])
        
        with tempfile.TemporaryDirectory() as tmpdir:
            tmpdir = Path(tmpdir)
            
            # Write the C++ code to a file
            cpp_file = tmpdir / f"{task_id}_{test_type}.cpp"
            cpp_file.write_text(code, encoding='utf-8')
            
            # Prepare compilation command
            output_file = tmpdir / f"{task_id}_{test_type}"
            
            # Base compilation command
            cmd = [
                'g++',
                '-std=c++17',
                '-O2',
                str(cpp_file),
                '-o', str(output_file),
                f'-I{self.utils_path}',
            ] + flags
            
            # Add benchmark library for performance tests
            if test_type == 'performance':
                cmd.extend(['-lbenchmark', '-lpthread'])
            
            try:
                result = subprocess.run(
                    cmd,
                    capture_output=True,
                    text=True,
                    timeout=30
                )
                
                if result.returncode == 0:
                    return True, None
                else:
                    return False, result.stderr
                    
            except subprocess.TimeoutExpired:
                return False, "Compilation timed out"
            except FileNotFoundError:
                return False, "g++ compiler not found. Please install g++."
            except Exception as e:
                return False, str(e)
    
    def test_problem(self, problem, test_type='correctness'):
        """Test compilation of a single problem."""
        
        task_id = problem.get('task_id', 'unknown')
        intrinsic = problem.get('intrinsic', 'AVX')
        
        # Get the test code
        code_field = f'test_{test_type}'
        code = problem.get(code_field, '')
        
        if not code:
            result = {
                'task_id': task_id,
                'test_type': test_type,
                'success': False,
                'error': f'No {code_field} field found'
            }
            self.results[test_type]['failed'].append(result)
            return result
        
        # Try to compile
        success, error = self.compile_code(code, task_id, test_type, intrinsic)
        
        result = {
            'task_id': task_id,
            'test_type': test_type,
            'intrinsic': intrinsic,
            'success': success,
            'error': error if not success else None
        }
        
        if success:
            self.results[test_type]['passed'].append(result)
        else:
            self.results[test_type]['failed'].append(result)
        
        return result
    
    def test_dataset(self, dataset_file, num_samples=5, test_both=True):
        """Test a random sample of problems from a dataset."""
        
        print(f"\n{'='*70}")
        print(f"Testing: {Path(dataset_file).name}")
        print(f"{'='*70}\n")
        
        # Load all problems
        problems = []
        with open(dataset_file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line:
                    problems.append(json.loads(line))
        
        # Sample randomly
        if len(problems) > num_samples:
            sampled = random.sample(problems, num_samples)
        else:
            sampled = problems
        
        print(f"Testing {len(sampled)} problems...\n")
        
        # Test each problem
        for i, problem in enumerate(sampled, 1):
            task_id = problem.get('task_id', f'problem_{i}')
            print(f"{i}. {task_id}")
            
            # Test correctness
            result = self.test_problem(problem, 'correctness')
            status = "✅ PASS" if result['success'] else "❌ FAIL"
            print(f"   Correctness: {status}")
            
            if not result['success'] and result['error']:
                # Show first few lines of error
                error_lines = result['error'].split('\n')[:3]
                for line in error_lines:
                    if line.strip():
                        print(f"     {line[:80]}")
            
            # Test performance if requested
            if test_both:
                result = self.test_problem(problem, 'performance')
                status = "✅ PASS" if result['success'] else "❌ FAIL"
                print(f"   Performance: {status}")
                
                if not result['success'] and result['error']:
                    error_lines = result['error'].split('\n')[:3]
                    for line in error_lines:
                        if line.strip():
                            print(f"     {line[:80]}")
            
            print()
    
    def print_summary(self):
        """Print overall summary of test results."""
        
        print("\n" + "="*70)
        print("SUMMARY")
        print("="*70)
        
        for test_type in ['correctness', 'performance']:
            passed = len(self.results[test_type]['passed'])
            failed = len(self.results[test_type]['failed'])
            total = passed + failed
            
            if total > 0:
                pass_rate = (passed / total) * 100
                print(f"\n{test_type.upper()} Tests:")
                print(f"  ✅ Passed: {passed}/{total} ({pass_rate:.1f}%)")
                print(f"  ❌ Failed: {failed}/{total}")
                
                if failed > 0:
                    print(f"\n  Failed tasks:")
                    for result in self.results[test_type]['failed']:
                        print(f"    - {result['task_id']}")


def main():
    # Paths
    base_dir = Path(__file__).parent
    utils_path = base_dir / 'simdbench' / 'utils'
    
    # Check if utils directory exists
    if not utils_path.exists():
        print(f"❌ ERROR: Utils directory not found at {utils_path}")
        print("   Make sure Random.h and allclose.h are in simdbench/utils/")
        sys.exit(1)
    
    # Initialize tester
    tester = CompilationTester(utils_path)
    
    # Test SimdBench dataset
    simdbench_avx = base_dir / 'data' / 'simdbench.jsonl'
    if simdbench_avx.exists():
        tester.test_dataset(simdbench_avx, num_samples=5, test_both=True)
    else:
        print(f"⚠️  SimdBench dataset not found at {simdbench_avx}")
    
    # Test TSVC cleaned dataset
    tsvc_clean = base_dir / 'tsvc_clean_cleaned.jsonl'
    if tsvc_clean.exists():
        tester.test_dataset(tsvc_clean, num_samples=5, test_both=True)
    else:
        print(f"⚠️  TSVC cleaned dataset not found at {tsvc_clean}")
        # Try uncleaned version
        tsvc_clean = base_dir / 'tsvc_clean.jsonl'
        if tsvc_clean.exists():
            print(f"   Using uncleaned version instead")
            tester.test_dataset(tsvc_clean, num_samples=5, test_both=True)
    
    # Print summary
    tester.print_summary()
    
    # Return exit code based on results
    total_failed = (len(tester.results['correctness']['failed']) + 
                   len(tester.results['performance']['failed']))
    
    if total_failed > 0:
        print(f"\n⚠️  {total_failed} compilation failures detected!")
        sys.exit(1)
    else:
        print("\n✅ All tests compiled successfully!")
        sys.exit(0)


if __name__ == '__main__':
    main()

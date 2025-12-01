#!/usr/bin/env python3
"""
Validation script for TSVC SIMD dataset
Checks format, required fields, and consistency
"""

import json
import sys
from pathlib import Path

def validate_jsonl(filepath, intrinsic_type="AVX"):
    """Validate TSVC JSONL dataset"""
    
    required_fields = [
        "task_id",
        "prompt", 
        "solution_scalar",
        "test_correctness",
        "test_performance",
        "intrinsic",
        "task",
        "entrypoint_scalar",
        "entrypoint_simd"
    ]
    
    errors = []
    warnings = []
    problems = []
    
    print(f"🔍 Validating: {filepath}")
    print(f"Expected intrinsic: {intrinsic_type}\n")
    
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if not line:
                    continue
                    
                try:
                    problem = json.loads(line)
                    problems.append(problem)
                    
                    # Check required fields
                    missing_fields = [f for f in required_fields if f not in problem]
                    if missing_fields:
                        errors.append(f"Line {line_num}: Missing fields: {missing_fields}")
                    
                    # Check intrinsic type
                    if problem.get("intrinsic") != intrinsic_type:
                        errors.append(f"Line {line_num}: Expected intrinsic '{intrinsic_type}', got '{problem.get('intrinsic')}'")
                    
                    # Check prompt contains intrinsic requirement
                    prompt = problem.get("prompt", "")
                    if intrinsic_type not in prompt and "AVX" not in prompt:
                        warnings.append(f"Line {line_num} ({problem.get('task_id')}): Prompt doesn't mention {intrinsic_type}")
                    
                    # Check for AVX header includes
                    if intrinsic_type == "AVX":
                        if "#include <immintrin.h>" not in prompt:
                            warnings.append(f"Line {line_num} ({problem.get('task_id')}): Missing AVX header in prompt")
                    
                    # Check entrypoints
                    if problem.get("entrypoint_simd") and not problem.get("entrypoint_simd").endswith("_simd"):
                        warnings.append(f"Line {line_num}: SIMD entrypoint should end with '_simd'")
                    
                    # Check scalar solution exists
                    if not problem.get("solution_scalar"):
                        errors.append(f"Line {line_num}: Empty solution_scalar")
                    
                    # Check test code exists
                    if not problem.get("test_correctness"):
                        errors.append(f"Line {line_num}: Empty test_correctness")
                    
                except json.JSONDecodeError as e:
                    errors.append(f"Line {line_num}: Invalid JSON - {e}")
                    
    except FileNotFoundError:
        print(f" File not found: {filepath}")
        return False
    
    # Print results
    print(f" Statistics:")
    print(f"  Total problems: {len(problems)}")
    print(f"  Unique task_ids: {len(set(p.get('task_id', '') for p in problems))}")
    print()
    
    if errors:
        print(f" Found {len(errors)} errors:")
        for error in errors[:10]:  # Show first 10
            print(f"  - {error}")
        if len(errors) > 10:
            print(f"  ... and {len(errors) - 10} more")
        print()
    
    if warnings:
        print(f"  Found {len(warnings)} warnings:")
        for warning in warnings[:10]:
            print(f"  - {warning}")
        if len(warnings) > 10:
            print(f"  ... and {len(warnings) - 10} more")
        print()
    
    if not errors and not warnings:
        print(" All checks passed!")
        return True
    elif not errors:
        print(" No errors found (only warnings)")
        return True
    else:
        print(" Validation failed")
        return False


def compare_with_scalar(avx_file, scalar_file):
    """Compare AVX and scalar versions for consistency"""
    
    print(f"\n Comparing AVX and Scalar datasets...")
    
    # Load both files
    avx_problems = {}
    scalar_problems = {}
    
    with open(avx_file, 'r') as f:
        for line in f:
            if line.strip():
                p = json.loads(line)
                # Extract base task_id (remove _AVX suffix)
                base_id = p['task_id'].replace('_AVX', '').replace('_SSE', '').replace('_SVE', '').replace('_Neon', '').replace('_RVV', '')
                avx_problems[base_id] = p
    
    with open(scalar_file, 'r') as f:
        for line in f:
            if line.strip():
                p = json.loads(line)
                scalar_problems[p['task_id']] = p
    
    # Check for matching problems
    avx_ids = set(avx_problems.keys())
    scalar_ids = set(scalar_problems.keys())
    
    print(f"  AVX problems: {len(avx_ids)}")
    print(f"  Scalar problems: {len(scalar_ids)}")
    print(f"  Common problems: {len(avx_ids & scalar_ids)}")
    
    if avx_ids - scalar_ids:
        print(f"\n  AVX problems without scalar version: {len(avx_ids - scalar_ids)}")
        for task_id in list(avx_ids - scalar_ids)[:5]:
            print(f"    - {task_id}")
    
    if scalar_ids - avx_ids:
        print(f"\n  Scalar problems without AVX version: {len(scalar_ids - avx_ids)}")
        for task_id in list(scalar_ids - avx_ids)[:5]:
            print(f"    - {task_id}")
    
    # Check if scalar solutions match
    mismatches = 0
    for task_id in avx_ids & scalar_ids:
        avx_scalar = avx_problems[task_id].get('solution_scalar', '')
        scalar_solution = scalar_problems[task_id].get('solution_scalar', '')
        
        if avx_scalar != scalar_solution:
            mismatches += 1
            if mismatches <= 3:
                print(f"\n  Scalar solution mismatch for {task_id}")
    
    if mismatches > 0:
        print(f"\n  Total scalar solution mismatches: {mismatches}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python validate_tsvc_dataset.py <tsvc_avx.jsonl> [tsvc_scalar.jsonl]")
        sys.exit(1)
    
    avx_file = sys.argv[1]
    
    # Validate AVX file
    is_valid = validate_jsonl(avx_file, intrinsic_type="AVX")
    
    # If scalar file provided, compare
    if len(sys.argv) >= 3:
        scalar_file = sys.argv[2]
        if Path(scalar_file).exists():
            compare_with_scalar(avx_file, scalar_file)
    
    sys.exit(0 if is_valid else 1)

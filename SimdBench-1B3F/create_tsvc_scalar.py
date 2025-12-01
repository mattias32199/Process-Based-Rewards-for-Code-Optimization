#!/usr/bin/env python3
"""
Create scalar version of TSVC dataset from AVX version
Removes SIMD-specific requirements and headers
"""

import json
import sys
import re

def convert_avx_to_scalar(avx_problem):
    """Convert an AVX problem to scalar version"""
    
    scalar_problem = avx_problem.copy()    
    # Update intrinsic type
    scalar_problem['intrinsic'] = 'scalar'
    
    # Update task_id (remove _AVX suffix)
    task_id = scalar_problem['task_id']
    scalar_problem['task_id'] = task_id.replace('_AVX', '').replace('_SSE', '').replace('_SVE', '').replace('_Neon', '').replace('_RVV', '')
    
    # Clean up prompt - remove SIMD-specific parts
    prompt = scalar_problem['prompt']
    
    # Remove AVX header includes
    prompt = re.sub(r'#include\s*<immintrin\.h>\s*\n', '', prompt)
    prompt = re.sub(r'#include\s*<x86intrin\.h>\s*\n', '', prompt)
    prompt = re.sub(r'#include\s*<xmmintrin\.h>\s*\n', '', prompt)
    prompt = re.sub(r'#include\s*<emmintrin\.h>\s*\n', '', prompt)
    prompt = re.sub(r'#include\s*<arm_sve\.h>\s*\n', '', prompt)
    prompt = re.sub(r'#include\s*<arm_neon\.h>\s*\n', '', prompt)
    prompt = re.sub(r'#include\s*<riscv_vector\.h>\s*\n', '', prompt)
    
    # Remove SIMD requirement text
    prompt = re.sub(
        r'The requirement is to implement the function with.*?intrinsics for parallelism\.\s*\n',
        '',
        prompt,
        flags=re.DOTALL
    )
    
    # Remove any remaining AVX/SIMD mentions in requirements
    prompt = re.sub(r'with AVX/AVX2.*?for parallelism\.?', '', prompt)
    prompt = re.sub(r'with SSE/SSE2.*?for parallelism\.?', '', prompt)
    prompt = re.sub(r'using.*?intrinsics', '', prompt)
    
    # Change function name from _simd to regular
    if scalar_problem.get('entrypoint_simd'):
        base_name = scalar_problem['entrypoint_simd'].replace('_simd', '')
        prompt = prompt.replace(scalar_problem['entrypoint_simd'], base_name)
    
    # Clean up extra newlines
    prompt = re.sub(r'\n\n\n+', '\n\n', prompt)
    
    scalar_problem['prompt'] = prompt.strip() + '\n'
    
    # Update entrypoint (remove _simd suffix)
    if scalar_problem.get('entrypoint_simd'):
        scalar_problem['entrypoint_simd'] = scalar_problem['entrypoint_simd'].replace('_simd', '')
    
    return scalar_problem


def create_scalar_dataset(avx_file, output_file):
    """Create scalar dataset from AVX dataset"""
    
    print(f"📖 Reading AVX dataset: {avx_file}")
    
    problems = []
    with open(avx_file, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            
            try:
                avx_problem = json.loads(line)
                scalar_problem = convert_avx_to_scalar(avx_problem)
                problems.append(scalar_problem)
            except json.JSONDecodeError as e:
                print(f"⚠️  Skipping invalid JSON line: {e}")
    
    print(f"✅ Converted {len(problems)} problems to scalar")
    
    # Write output
    print(f"💾 Writing scalar dataset: {output_file}")
    with open(output_file, 'w', encoding='utf-8') as f:
        for problem in problems:
            f.write(json.dumps(problem) + '\n')
    
    print(f"✅ Done! Created {output_file}")
    
    # Show sample
    if problems:
        print("\n📝 Sample scalar problem:")
        sample = problems[0]
        print(f"  task_id: {sample['task_id']}")
        print(f"  intrinsic: {sample['intrinsic']}")
        print(f"  entrypoint: {sample.get('entrypoint_simd', 'N/A')}")
        print(f"\n  Prompt preview:")
        print("  " + "\n  ".join(sample['prompt'][:300].split('\n')))
        if len(sample['prompt']) > 300:
            print("  ...")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python create_tsvc_scalar.py <input_avx.jsonl> <output_scalar.jsonl>")
        print("\nExample:")
        print("  python create_tsvc_scalar.py tsvc_avx_only.jsonl tsvc_scalar.jsonl")
        sys.exit(1)
    
    avx_file = sys.argv[1]
    output_file = sys.argv[2]
    
    create_scalar_dataset(avx_file, output_file)

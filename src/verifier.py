# src/verifier.py
from src.correctness import verify_simd_correctness
from src.speedup import verify_speedup

def verify(task: dict, sol_simd: str) -> dict:
    """
    verif should contain test_info, outcome, and error messages (if appropriate)
    """
    verif = verify_simd_correctness(task, sol_simd)
    if verif['compiles'] and verif['correct']:
        verif = verify_speedup(task, sol_simd)
        if verif['success']:
            verif['correct'] = True
        else:
            verif['correct'] = False
        # verif['correct'] = True

    verif['correct_format'] = True
    return verif

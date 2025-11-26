# src/verifier.py
from correctness import verify_simd_correctness
from speedup import verify_speedup

def verify(task: dict, sol_simd: str) -> dict:
    correctness_feedback = verify_simd_correctness(task, sol_simd)
    feedback = "Correctness test:\n" + str(correctness_feedback['feedback']) + '\n'
    if correctness_feedback['correctness']:
        speedup_feedback = verify_speedup(task, sol_simd)
        feedback += '\n' + speedup_feedback['feedback']
    else:
        speedup_feedback = None
    return {
        'compiles': correctness_feedback['compiles'],
        'correctness': correctness_feedback['correctness'],
        'feedback': feedback
    }

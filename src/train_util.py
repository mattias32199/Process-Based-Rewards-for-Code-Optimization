# src/train_util.py
import re

# PROMPT TEMPLATES

# system prompt with COT
from typing import Dict


SYSTEM_PROMPT_WITH_COT = """You are a SIMD optimization expert. You MUST structure your response with these exact tags:

<think>
Explain your optimization strategy, register usage, and approach here.
</think>

<simd_code>
Your complete SIMD implementation here.
<simd_code>"""

# system prompt without COT
SYSTEM_PROMPT_NO_COT = """You are a SIMD optimization expert. You MUST structure your response with these exact tags:

<simd_code>
Your complete SIMD implementation here.
</simd_code>"""

# base prompt is the same w/ and w/o cot
BASE_PROMPT = """You are given the following scalar baseline:

{SCALAR_SOLUTION}

Your task:
{INITIAL_PROMPT}

Target architecture: {INTRINSIC}

Example response:
<vectorization_analysis>
Using {INTRINSIC} 256-bit registers (__m256) to process 8 floats per iteration...
</vectorization_analysis>

<simd_code>
#include <immintrin.h>
void example_simd(float *a, const float *b, size_t n) {{
    // implementation
}}
</simd_code>
{PREVIOUS_ATTEMPTS}"""

FEEDBACK_TEMPLATE="""Here are your previous attempts:
{feedback}

Restart your reasoning process and generate new, complete code."""


# feedback messages
FEEDBACK_MESSAGES = {
    'parsing_error': "Your previous answer failed to be parsed due to not adhering to the desired formatting. Here is the error message:\n{error}",
    'compilation_error': "Your previous answer failed to compile. Here is the error message:\n{error}",
    'runtime_error': "Your previous answer compiled successfully but had runtime errors. Here is the error message:\n{error}",
    'correctness_error': "Your previous answer was functionally incorrect and did not produce the expected results.",
    'no_intrinsic': "Your previous answer compiled but does not use {intrinsic} intrinsics. You must use SIMD intrinsics for this task.",
    'correct_slow': "Your previous answer was functionally correct but can be made faster. Here is the speedup you achieved relative to the baseline: {speedup:.2f}x",
    'correct_fast': "Your previous answer achieved {speedup:.2f}x speedup. Try to optimize further.",
}


# FUNCTIONS
def get_system_prompt(use_cot: bool=False) -> str:
    """
    Selects system prompt based on cot
    """
    return SYSTEM_PROMPT_WITH_COT if use_cot else SYSTEM_PROMPT_NO_COT

def construct_feedback(feedback_buffer: dict) -> list[str]:
    feedback_parts = []
    result = feedback_buffer['result']
    feedback_parts.append(FEEDBACK_MESSAGES[result])
    if feedback_buffer.get('traceback'):
        feedback_parts.append(feedback_buffer['traceback'])
    return feedback_parts

def construct_previous_attempts(context_buffer: dict, turn: int, use_cot:bool=False) -> str:
    prev_attempts_parts = []
    for t in range(turn):
        turn_context = context_buffer['turns'][t]
        prev_attempts_parts.append(f"<attempt_{t + 1}>") # denote each attempt
        if use_cot and turn_context.get('cot'):
            prev_attempts_parts.append(f"<think>\n{turn_context['cot']}\n</think>") # cot
        prev_attempts_parts.append(f"<simd_code>\n{turn_context['solution']}\n</simd_code>") # simd solution
        feedback_parts = construct_feedback(context_buffer['feedback'])
        prev_attempts_parts.extend(feedback_parts) # unbound error

        prev_attempts_parts.append(f"\n{turn_context['feedback']}")
        prev_attempts_parts.append(f"</attempt_{t + 1}>\n")

    prev_attempts = FEEDBACK_TEMPLATE.format(
        FEEDBACK="\n".join(prev_attempts_parts)
    )

    return prev_attempts


def construct_prompt(context_buffer: dict, turn: int, use_cot: bool) -> tuple[str, str]:
    """
    Top-level function for system and user prompts
    """
    system_prompt = get_system_prompt(use_cot)

    task = context_buffer['task']
    if turn == 0:
        user_prompt = BASE_PROMPT.format(
            SCALAR_SOLUTION=task['scalar_solution'],
            INITIAL_PROMPT=task['prompt'],
            INTRINSIC=task['intrinsic'],
            # ENTRYPOINT=task['simd_entrypoint'],
            PREVIOUS_ATTEMPTS=""
        )
    else: # if not first turn -> inject feedback from previous attempts
        prev_attempts = construct_previous_attempts(context_buffer, turn, use_cot)

        user_prompt = BASE_PROMPT.format(
            SCALAR_SOLUTION=context_buffer['task']['scalar_solution'],
            INITIAL_PROMPT=context_buffer['task']['prompt'],
            PREVIOUS_ATTEMPTS=prev_attempts
        )
    return user_prompt, system_prompt

def parse_response(response:str, simd_entrypoint:str, use_cot: bool=False) -> dict:
    cot, simd_solution = None, None
    parse_solution, parse_think = False, False
    correct_format = True
    # parse for expected tags
    code_pattern = r'<simd_code>\s*(.*?)\s*</simd_code>'
    code_match = re.search(code_pattern, response, re.DOTALL)
    if code_match:
        parse_solution = True
        simd_solution = code_match.group(1).strip()
        if use_cot:
            cot_pattern = r'<vectorization_analysis>\s*(.*?)\s*</vectorization_analysis>'
            cot_match = re.search(cot_pattern, response, re.DOTALL)
            if cot_match:
                parse_think = True
                cot = cot_match.group(1).strip()
            else:
                correct_format = False
    else:
        correct_format = False
        code_block_pattern = re.compile(r"```[^\n]*\n(.*?)```", re.DOTALL)
        matches = list(code_block_pattern.finditer(response))

        for match in matches:
            potential_code = match.group(1).strip()
            if simd_entrypoint in potential_code:
                simd_solution = potential_code
                parse_solution = True
                break

    if not parse_solution and (simd_entrypoint in response):
        # Try to extract function definition heuristically
        # Look for the function signature and grab everything until next function or end
        func_pattern = rf'(void\s+{re.escape(simd_entrypoint)}\s*\([^)]*\)\s*\{{[^}}]*\}})'
        func_match = re.search(func_pattern, response, re.DOTALL)
        if func_match:
            simd_solution = func_match.group(1).strip()
            parse_solution = True


    return {
        'simd_solution': simd_solution,
        'cot': cot,
        'correct_format': correct_format,
        'parse_solution': parse_solution,
        'parse_cot': parse_think
    }

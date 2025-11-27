# src/train_util.py
import re

# PROMPT TEMPLATES

# system prompt with COT
SYSTEM_PROMPT_WITH_COT = """You are a SIMD optimization expert. You MUST structure your response with these exact tags:

<think>
Explain your optimization strategy, register usage, and approach here.
</think>

<simd_code>
Your complete SIMD implementation here.
</simd_code>"""

# system prompt without COT
SYSTEM_PROMPT_NO_COT = """You are a SIMD optimization expert. You MUST structure your response with these exact tags:

<simd_code>
Your complete SIMD implementation here.
</simd_code>"""

# base prompt is the same w/ and w/o cot
BASE_PROMPT_WITH_COT = """You are given the following scalar baseline:

{SCALAR_SOLUTION}

Your task:
{INITIAL_PROMPT}

Target architecture: {INTRINSIC}

Example response:
<think>
Using {INTRINSIC} 256-bit registers (__m256) to process 8 floats per iteration...
</think>

<simd_code>
#include <immintrin.h>
void example_simd(float *a, const float *b, size_t n) {{
    // implementation
}}
</simd_code>
{PREVIOUS_ATTEMPTS}"""

# base prompt is the same w/ and w/o cot
BASE_PROMPT_NO_COT = """You are given the following scalar baseline:

{SCALAR_SOLUTION}

Your task:
{INITIAL_PROMPT}

Target architecture: {INTRINSIC}

Example response:
<simd_code>
#include <immintrin.h>
void example_simd(float *a, const float *b, size_t n) {{
    // implementation
}}
</simd_code>
{PREVIOUS_ATTEMPTS}"""

FEEDBACK_TEMPLATE="""Here are your previous attempts:
{FEEDBACK}

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

def get_base_prompt(use_cot: bool=False) -> str:
    return BASE_PROMPT_WITH_COT if use_cot else BASE_PROMPT_NO_COT

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
        feedback_parts = construct_feedback(turn_context['feedback'])
        prev_attempts_parts.extend(feedback_parts)
        prev_attempts_parts.append(f"</attempt_{t + 1}>\n")

    prev_attempts = FEEDBACK_TEMPLATE.format(
        FEEDBACK="\n".join(prev_attempts_parts)
    )

    return prev_attempts


def construct_user_prompt(context_buffer: dict, turn: int, use_cot: bool) -> str:
    """
    Top-level function for system and user prompts
    """
    # Get system and base prompt
    base_prompt = get_base_prompt(use_cot)

    # Fill in base prompt
    task = context_buffer['task']
    if turn == 0:
        user_prompt = base_prompt.format(
            SCALAR_SOLUTION=task['scalar_solution'],
            INITIAL_PROMPT=task['prompt'],
            INTRINSIC=task['intrinsic'],
            # ENTRYPOINT=task['simd_entrypoint'],
            PREVIOUS_ATTEMPTS=""
        )
    else: # if not first turn -> inject feedback from previous attempts
        prev_attempts = construct_previous_attempts(context_buffer, turn, use_cot)

        user_prompt = base_prompt.format(
            SCALAR_SOLUTION=context_buffer['task']['scalar_solution'],
            INITIAL_PROMPT=context_buffer['task']['prompt'],
            INTRINSIC=task['intrinsic'],
            PREVIOUS_ATTEMPTS=prev_attempts
        )
    return user_prompt

def parse_response(response:str, simd_entrypoint:str, use_cot: bool=False) -> dict:
    cot, simd_solution = None, None
    parse_solution, parse_cot = False, False
    correct_format = True
    feedback = []

    # parse for simd_code tags
    code_tag_pattern = re.compile(r'<simd_code>\s*(.*?)\s*</simd_code>', re.DOTALL | re.IGNORECASE)
    code_tag_match = code_tag_pattern.search(response)
    if code_tag_match:
        extracted = code_tag_match.group(1).strip()
        if extracted:
            simd_solution = extracted
            parse_solution = simd_entrypoint in extracted or simd_entrypoint == ""  # allow empty entrypoint check if caller passes ""
            if not parse_solution:
                feedback.append("simd_entrypoint not found inside <simd_code> block")
                correct_format = False
        else:
            feedback.append("<simd_code> block present but empty")
            correct_format = False
    else:
        feedback.append("Missing required <simd_code> block")
        correct_format = False

    # parse for think tags
    if use_cot:
        think_pattern = re.compile(r'<think>\s*(.*?)\s*</think>', re.DOTALL | re.IGNORECASE)
        think_match = think_pattern.search(response)
        if think_match:
            cot = think_match.group(1).strip()
            parse_cot = bool(cot)
            if not parse_cot:
                feedback.append("<think> block present but empty")
                correct_format = False
        else:
            feedback.append("Missing required <think> block")
            correct_format = False

    # check if solution is empty or whitespace
    if simd_solution is not None:
        simd_solution = simd_solution.strip()
        if not simd_solution:
            parse_solution = False
            correct_format = False
            simd_solution = None
    return {
        'simd_solution': simd_solution,
        'cot': cot,
        'correct_format': correct_format,
        'parse_solution': parse_solution,
        'parse_cot': parse_cot,
        'feedback': feedback
    }


if __name__ == "__main__":
    print("Testing train_util.py...\n")

    # Simple test data
    simple_task = {
        'scalar_solution': 'void add_scalar(float *a, float *b, int n) { }',
        'prompt': 'Implement: void add_simd(float *a, float *b, int n) { }',
        'intrinsic': 'AVX',
    }

    # Test 1: System prompts
    print("Test 1: System prompts")
    sys_cot = get_system_prompt(use_cot=True)
    sys_no_cot = get_system_prompt(use_cot=False)
    assert '<think>' in sys_cot and '<think>' not in sys_no_cot
    print("✓ System prompts work\n")

    # Test 2: Turn 0 prompt
    print("Test 2: Turn 0 prompt")
    ctx = {'task': simple_task, 'turns': []}
    user = construct_user_prompt(ctx, turn=0, use_cot=True)
    assert 'AVX' in user and 'add_scalar' in user
    print("✓ Turn 0 prompt works\n")

    # Test 3: Feedback construction
    print("Test 3: Feedback construction")
    fb = construct_feedback({'result': 'compilation_error', 'traceback': 'error msg'})
    assert len(fb) == 2 and 'failed to compile' in fb[0]
    print("✓ Feedback construction works\n")

    # Test 4: Turn 1 with history
    print("Test 4: Turn 1 with history")
    ctx['turns'].append({
        'solution': 'void add_simd() { }',
        'cot': 'My plan',
        'feedback': {'result': 'compilation_error'}
    })
    user = construct_user_prompt(ctx, turn=1, use_cot=True)
    assert '<attempt_1>' in user and 'failed to compile' in user
    print("✓ Turn 1 with history works\n")

    # Test 5: Parse perfect response
    print("Test 5: Parse perfect response")
    response = "<think>My plan</think>\n<simd_code>void add_simd() { }</simd_code>"
    result = parse_response(response, 'add_simd', use_cot=True)
    assert result['parse_solution'] and result['parse_cot'] and result['correct_format']
    print("✓ Parser works on perfect format\n")

    # Test 6: Parse markdown fallback
    print("Test 6: Parse markdown fallback")
    response_md = "```cpp\nvoid add_simd() { }\n```"
    result_md = parse_response(response_md, 'add_simd', use_cot=False)
    assert result_md['parse_solution'] and not result_md['correct_format']
    print("✓ Parser works on markdown fallback\n")

    # Test 7: Parse failure
    print("Test 7: Parse failure")
    result_bad = parse_response("I don't know", 'add_simd', use_cot=False)
    assert not result_bad['parse_solution']
    print("✓ Parser handles failure correctly\n")

    print("=" * 50)
    print("All tests passed! ✓")
    print("=" * 50)

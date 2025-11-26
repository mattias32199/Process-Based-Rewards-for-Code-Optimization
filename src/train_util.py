# src/train_util.py

BASE_PROMPT = """
You are given the following architecture:
{SCALAR_SOLUTION}

{INITIAL_PROMPT}
{PREVIOUS_ATTEMPTS}
"""

FEEDBACK_TEMPLATE = """
Here are your previous attempts:

{FEEDBACK}
"""

def construct_prompt(context_buffer, turn: int) -> str:
    if turn == 0:
        return BASE_PROMPT.format(
            SCALAR_SOLUTION=context_buffer['task']['scalar_solution'],
            INITIAL_PROMPT=context_buffer['task']['prompt'],
            PREVIOUS_ATTEMPTS=""
        )
    else:
        total_feedback = ""
        for turn in range(turn - 1): # stop before current turn
            total_feedback += f'Attempt {turn + 1}\n'
            total_feedback += f'{context_buffer['turns']['solution']}\n' # append simd solution
            # total_feedback += f'{context_buffer['turns']['cot']}'
            total_feedback += f'{context_buffer['turns']['feedback']}'
            total_feedback += '\n'

        previous_attempts = FEEDBACK_TEMPLATE.format(FEEDBACK=total_feedback)

        return BASE_PROMPT.format(
            SCALAR_SOLUTION=context_buffer['task']['scalar_solution'],
            INITIAL_PROMPT=context_buffer['task']['prompt'],
            PREVIOUS_ATTEMPTS=previous_attempts
        )
    raise

def parse_response(response):
    raise NotImplementedError

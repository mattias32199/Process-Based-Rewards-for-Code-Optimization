import numpy as np
import json

### remove newlines from tsvc bench
# fixed_lines = []
# with open('data/raw/tsvc_simdbench.jsonl', 'r') as f:
#     for line in f:
#         line = line.strip()
#         if not line:
#             continue
#         if line.startswith('//'):
#             line = line[2:].strip()
#         try:
#             obj = json.loads(line)
#             fixed_lines.append(json.dumps(obj) + '\n')
#         except Exception as e:
#             print(f"Error has occurred: {e}")
#             fixed_line = line
#             fixed_line = fixed_line.replace('\t', '\\t')
#             fixed_line = fixed_line.replace('\xa0', ' ')
#             fixed_line = fixed_line.replace('\r', '\\r')

#             try:
#                 obj = json.loads(fixed_line)
#                 fixed_lines.append(json.dumps(obj) + '\n')
#                 print('\tError has been fixed')
#             except Exception as e:
#                 print(f"Error persists: {e}")
#                 print(f'line:\n{line}')
#                 break
# print('Writing fixed file')
# with open('data/processed/tsvc-scalar.jsonl', 'w') as f:
#     f.writelines(fixed_lines)

### check processed file for issues
# try:
#     bad_lines = []
#     with open('data/processed/tsvc-scalar.jsonl', 'r') as f:
#         for i, line in enumerate(f, start=1):
#             try:
#                 json.loads(line)
#             except json.JSONDecodeError as e:
#                 print(f"Line {i} failed: {e}")
#                 bad_lines.append((i, line[:200]))  # preview first 200 chars

#     print(f"Found {len(bad_lines)} bad lines")
# except Exception as e:
#     print(f'Error opening processed file {e}')

# remove duplicates
new = []
task_ids = []
with open('data/processed/tsvc-scalar.jsonl', 'r') as f:
    for line in f:
        task = json.loads(line)
        task_id = task['task_id']
        if task_id in task_ids:
            print(f'Duplicate task id found: {task_id}')
        else:
            task_ids.append(task_id)
            new.append(json.dumps(task) + '\n')
print('Writing fixed file')
print(len(new))
with open('data/processed/tsvc-scalar.jsonl', 'w') as f:
    f.writelines(new)

# tsvc_tasks = []
# task_ids = []
# with open('data/processed/tsvc-scalar.jsonl', 'r') as f:
#     for line in f:
#         tsvc_tasks.append(json.loads(line))
#         print(line)
#         break

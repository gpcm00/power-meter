import os
import re

SRC_DIR = "../src"
OUTPUT_FILE = "todo_list.txt"

TODO_PATTERN = re.compile(r"TODO[:\s]*(.*)")

todos = []

for root, _, files in os.walk(SRC_DIR):
    for file in files:
        if file.endswith((".c", ".h")):
            file_path = os.path.join(root, file)
            with open(file_path, "r", encoding="utf-8") as f:
                for line in f:
                    match = TODO_PATTERN.search(line)
                    if match:
                        todos.append(f"TODO: {match.group(1).strip()} [{file_path}]")

with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
    for todo in todos:
        f.write(todo + "\n")

print(f"Collected {len(todos)} TODOs into {OUTPUT_FILE}")

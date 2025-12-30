#!/bin/bash

# iterates over the .cpp and .hh files in src/ and tests/ and count the number of non-empty lines per file
# also exclude lines with 2 or fewer characters (to ignore lines with just braces or very short comments)
# return the total number of lines over all files
# does the same thing for the markdown files in notes/
total_code_lines=0
total_note_lines=0
for file in src/**/*.cpp src/**/*.hh tests/**/*.cpp tests/**/*.hh; do
    if [[ -f "$file" ]]; then
        line_count=$(grep -cve '^\s*$' "$file" | grep -cvE '^\s{0,2}$' "$file")
        total_code_lines=$((total_code_lines + line_count))
    fi
done
for file in notes/*.md; do
    if [[ -f "$file" ]]; then
        line_count=$(grep -cve '^\s*$' "$file" | grep -cvE '^\s{0,2}$' "$file")
        total_note_lines=$((total_note_lines + line_count))
    fi
done
echo "Total codefile lines: $total_code_lines"
echo "Total note lines: $total_note_lines"
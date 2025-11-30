#!/bin/bash

# iterate over the .cpp and .hh files in src, and count the number of non-empty lines in each file
# also exclude lines with 2 or fewer characters (to ignore lines with just braces or very short comments)
# return the total number of lines over all files
total_lines=0
for file in src/**/*.cpp src/**/*.hh; do
    if [[ -f "$file" ]]; then
        line_count=$(grep -cve '^\s*$' "$file" | grep -cvE '^\s{0,2}$' "$file")
        total_lines=$((total_lines + line_count))
    fi
done
echo "Total lines: $total_lines"
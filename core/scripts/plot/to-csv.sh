#!/bin/bash

# Flags
remove_ansi_only=false
quiet=false

# Argument Parsing
while [[ "$1" == --* ]]; do
    case "$1" in
        --remove-ansi-only) remove_ansi_only=true ;;
        --quiet) quiet=true ;;
        *) echo "Unknown option: $1" && exit 1 ;;
    esac
    shift
done

if [ $# -eq 0 ]; then
    echo "Usage: $0 [--remove-ansi-only] [--quiet] <input_file>"
    exit 1
fi

input_file="$1"

if [ ! -f "$input_file" ]; then
    echo "Error: File '$input_file' not found!"
    exit 1
fi

filename=$(basename "$input_file")
name="${filename%.*}"
temp_file=$(mktemp)

cp "$input_file" "$temp_file"

# Remove ANSI escape codes
sed -i 's/\x1B\[[0-9;]*[a-zA-Z]//g' "$temp_file"

if $remove_ansi_only; then
    cleaned_file="${name}_noansi.txt"
    mv "$temp_file" "$cleaned_file"
    if ! $quiet; then
        echo "ANSI codes removed."
        echo "Output file: $cleaned_file"
    fi
    exit 0
fi

# Default processing
sed -i '/(RTP/!d' "$temp_file"
sed -i 's/.*(RTP/RTP/' "$temp_file"
sed -i 's/RTP=*//' "$temp_file"
sed -i 's/%.*//g' "$temp_file"


output_file="${name}_raw.csv"
echo "RTP" > "$output_file"
cat "$temp_file" >> "$output_file"
rm "$temp_file"


if ! $quiet; then
    echo "Processing complete!"
    echo "Input file: $input_file"
    echo "Output file: $output_file"
    echo "Processed $(wc -l < "$output_file") lines (including header)"
fi

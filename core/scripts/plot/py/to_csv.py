import os
import re
import pandas as pd

def remove_ansi_codes(text):
    ansi_escape = re.compile(r'\x1B\[[0-9;]*[a-zA-Z]')
    return ansi_escape.sub('', text)

def extract_rtp_lines(text):
    lines = text.splitlines()
    cleaned = []
    for line in lines:
        if '(RTP' in line:
            line = line.split('(RTP')[-1]
            line = re.sub(r'RTP=*', '', line)
            line = re.sub(r'%.*', '', line)
            line = line.lstrip("= ").strip()
            cleaned.append(line.strip())
    return cleaned

def get_rtp_dataframe(input_file):
    if not os.path.isfile(input_file):
        raise FileNotFoundError(f"File '{input_file}' not found.")

    with open(input_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    content = remove_ansi_codes(content)
    cleaned_lines = extract_rtp_lines(content)

    if not cleaned_lines:
        raise ValueError("No valid RTP lines found in the log.")

    return pd.DataFrame({"RTP": cleaned_lines})

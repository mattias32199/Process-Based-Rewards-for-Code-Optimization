#!/usr/bin/env python3
"""
Clean Unicode characters from TSVC dataset files.
Replaces non-breaking spaces and other problematic Unicode with regular ASCII.
"""

import json
import sys
import re
from pathlib import Path


def clean_string(text):
    """Remove problematic Unicode characters from a string."""
    if not isinstance(text, str):
        return text
    
    # Replace non-breaking space (U+00A0) with regular space
    text = text.replace('\u00a0', ' ')
    
    # Replace other common problematic Unicode whitespace characters
    text = text.replace('\u2009', ' ')  # Thin space
    text = text.replace('\u200a', ' ')  # Hair space
    text = text.replace('\u202f', ' ')  # Narrow no-break space
    text = text.replace('\u205f', ' ')  # Medium mathematical space
    text = text.replace('\u3000', ' ')  # Ideographic space
    
    # Replace various types of hyphens/dashes with ASCII hyphen/minus
    text = text.replace('\u2010', '-')  # Hyphen
    text = text.replace('\u2011', '-')  # Non-breaking hyphen
    text = text.replace('\u2012', '-')  # Figure dash
    text = text.replace('\u2013', '-')  # En dash
    text = text.replace('\u2014', '-')  # Em dash
    text = text.replace('\u2015', '-')  # Horizontal bar
    
    # Replace smart quotes with regular quotes
    text = text.replace('\u2018', "'")  # Left single quote
    text = text.replace('\u2019', "'")  # Right single quote
    text = text.replace('\u201c', '"')  # Left double quote
    text = text.replace('\u201d', '"')  # Right double quote
    
    return text


def clean_dict(obj):
    """Recursively clean all strings in a dictionary."""
    if isinstance(obj, dict):
        return {k: clean_dict(v) for k, v in obj.items()}
    elif isinstance(obj, list):
        return [clean_dict(item) for item in obj]
    elif isinstance(obj, str):
        return clean_string(obj)
    else:
        return obj


def clean_jsonl_file(input_file, output_file=None):
    """Clean a JSONL file by removing problematic Unicode characters."""
    
    input_path = Path(input_file)
    if output_file is None:
        output_file = input_path.parent / f"{input_path.stem}_cleaned{input_path.suffix}"
    
    output_path = Path(output_file)
    
    print(f"🔍 Reading: {input_path}")
    
    cleaned_count = 0
    total_count = 0
    unicode_found = 0
    
    with open(input_path, 'r', encoding='utf-8') as f_in, \
         open(output_path, 'w', encoding='utf-8') as f_out:
        
        for line_num, line in enumerate(f_in, 1):
            line = line.strip()
            if not line:
                continue
            
            total_count += 1
            
            try:
                # Parse JSON
                problem = json.loads(line)
                
                # Clean the problem recursively
                cleaned_problem = clean_dict(problem)
                
                # Check if anything changed
                original_str = json.dumps(problem, ensure_ascii=False)
                cleaned_str = json.dumps(cleaned_problem, ensure_ascii=False)
                
                # Check for specific Unicode characters in original
                has_unicode = ('\u00a0' in original_str or 
                              '\u2009' in original_str or
                              '\u2018' in original_str or 
                              '\u2019' in original_str or
                              '\u201c' in original_str or 
                              '\u201d' in original_str)
                
                if has_unicode:
                    unicode_found += 1
                    
                if original_str != cleaned_str:
                    cleaned_count += 1
                
                # Write cleaned JSON (ensure_ascii=False to preserve other Unicode properly)
                f_out.write(json.dumps(cleaned_problem, ensure_ascii=False) + '\n')
                
            except json.JSONDecodeError as e:
                print(f"⚠️  Error on line {line_num}: {e}")
                continue
    
    print(f"✅ Found Unicode in {unicode_found}/{total_count} problems")
    print(f"✅ Cleaned {cleaned_count}/{total_count} problems")
    print(f"💾 Saved to: {output_path}")
    
    return output_path


def main():
    if len(sys.argv) < 2:
        print("Usage: python clean_unicode_tsvc.py <input.jsonl> [output.jsonl]")
        print("\nExample:")
        print("  python clean_unicode_tsvc.py tsvc_clean.jsonl")
        print("  python clean_unicode_tsvc.py tsvc_clean.jsonl tsvc_fixed.jsonl")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None
    
    clean_jsonl_file(input_file, output_file)


if __name__ == "__main__":
    main()

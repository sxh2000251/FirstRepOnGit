import re

def remove_comments(code):
    # Remove single-line comments
    code = re.sub(r'#.*', '', code)
    # Remove multi-line comments
    code = re.sub(r'"""(.*?)"""', '', code, flags=re.DOTALL)
    code = re.sub(r"'''(.*?)'''", '', code, flags=re.DOTALL)
    return code.strip()

if __name__ == "__main__":
    with open(".clang-format", "r", encoding='utf-8') as file:
        code = file.read()
    cleaned_code = remove_comments(code)
    with open("output.py", "w") as file:
        file.write(cleaned_code)
#!/usr/bin/env python3
"""
check_c89.py - C89 Conformance & Multi-Target Build Checker for DCode IDE.

Supported targets:
  - Linux (GCC with -std=c89 -pedantic -Wall -Wextra)
  - Watcom DOS (wcc386 with -za -w4 via makefile.watcom-linux / makefile)

Capabilities:
  1. Static scan of C/H files for strict C89 violations.
  2. Auto-fix mechanical violations (C++ comments, enum trailing commas, EOF newlines).
  3. Execution of makefiles for both targets with structured diagnostic reporting.
"""

import os
import sys
import re
import glob
import subprocess
import argparse

# Colors for terminal output
COLOR_RED = "\033[91m"
COLOR_YELLOW = "\033[93m"
COLOR_GREEN = "\033[92m"
COLOR_CYAN = "\033[96m"
COLOR_BOLD = "\033[1m"
COLOR_RESET = "\033[0m"


def find_repo_root():
    """Finds root of repo containing makefile or makefile.linux."""
    cur = os.path.abspath(os.getcwd())
    while cur != os.path.dirname(cur):
        if os.path.exists(os.path.join(cur, "makefile.linux")) or os.path.exists(os.path.join(cur, "makefile")):
            return cur
        cur = os.path.dirname(cur)
    return os.path.abspath(os.getcwd())


def get_source_files(root_dir):
    """Finds all relevant .c and .h files in the repository."""
    patterns = ["core/**/*.[ch]", "deps/**/*.[ch]", "app/**/*.[ch]", "hal/**/*.[ch]", "platform/**/*.[ch]"]
    files = []
    for pattern in patterns:
        for f in glob.glob(os.path.join(root_dir, pattern), recursive=True):
            rel = os.path.relpath(f, root_dir)
            if "external" not in rel and "bin" not in rel and ".git" not in rel:
                files.append(rel)
    return sorted(files)


def convert_cpp_comments(content):
    """
    Converts C++ single-line comments (// ...) to C89 block comments (/* ... */).
    Safely ignores comments within double-quoted strings, char literals,
    and already inside /* ... */ block comments.
    """
    out = []
    i = 0
    n = len(content)
    NORMAL, IN_STRING, IN_CHAR, IN_BLOCK_COMMENT, IN_LINE_COMMENT = 0, 1, 2, 3, 4
    state = NORMAL

    while i < n:
        if state == NORMAL:
            if i + 1 < n and content[i:i+2] == '/*':
                state = IN_BLOCK_COMMENT
                out.append('/*')
                i += 2
            elif i + 1 < n and content[i:i+2] == '//':
                state = IN_LINE_COMMENT
                out.append('/*')
                i += 2
            elif content[i] == '"':
                state = IN_STRING
                out.append('"')
                i += 1
            elif content[i] == "'":
                state = IN_CHAR
                out.append("'")
                i += 1
            else:
                out.append(content[i])
                i += 1
        elif state == IN_STRING:
            if content[i] == '\\':
                out.append(content[i])
                if i + 1 < n:
                    out.append(content[i+1])
                    i += 2
                else:
                    i += 1
            elif content[i] == '"':
                out.append('"')
                state = NORMAL
                i += 1
            else:
                out.append(content[i])
                i += 1
        elif state == IN_CHAR:
            if content[i] == '\\':
                out.append(content[i])
                if i + 1 < n:
                    out.append(content[i+1])
                    i += 2
                else:
                    i += 1
            elif content[i] == "'":
                out.append("'")
                state = NORMAL
                i += 1
            else:
                out.append(content[i])
                i += 1
        elif state == IN_BLOCK_COMMENT:
            if i + 1 < n and content[i:i+2] == '*/':
                out.append('*/')
                state = NORMAL
                i += 2
            else:
                out.append(content[i])
                i += 1
        elif state == IN_LINE_COMMENT:
            if content[i] in ('\r', '\n'):
                ws = []
                while out and out[-1] in (' ', '\t'):
                    ws.append(out.pop())
                if out and out[-1] not in (' ', '*', '/'):
                    out.append(' ')
                out.append('*/')
                if content[i] == '\r':
                    if i + 1 < n and content[i+1] == '\n':
                        out.append('\r\n')
                        i += 2
                    else:
                        out.append('\r')
                        i += 1
                else:
                    out.append('\n')
                    i += 1
                state = NORMAL
            elif i + 1 < n and content[i:i+2] == '*/':
                out.append('* /')
                i += 2
            else:
                out.append(content[i])
                i += 1

    if state == IN_LINE_COMMENT:
        ws = []
        while out and out[-1] in (' ', '\t'):
            ws.append(out.pop())
        if out and out[-1] not in (' ', '*', '/'):
            out.append(' ')
        out.append('*/\n')

    return ''.join(out)


def remove_enum_trailing_commas(content):
    """Removes illegal trailing commas from enum declarations."""
    def replacer(match):
        block = match.group(0)
        return re.sub(r',(\s*(?:/\*.*?\*/\s*)*\})', r'\1', block)
    return re.sub(r'enum\s*[\w\s]*\{[^}]+\}', replacer, content)


def scan_file(filepath, full_path):
    """Scans a file for C89 violations."""
    violations = []
    try:
        with open(full_path, "r", encoding="latin-1") as f:
            content = f.read()
    except Exception as e:
        return [f"Could not read file: {e}"]

    # 1. EOF newline check
    if not content.endswith("\n"):
        violations.append((0, "missing-eof-newline", "File does not end with a newline (causes Watcom W138)"))

    # 2. C++ style comments
    i = 0
    n = len(content)
    line_num = 1
    NORMAL, IN_STRING, IN_CHAR, IN_BLOCK_COMMENT = 0, 1, 2, 3
    state = NORMAL
    while i < n:
        ch = content[i]
        if ch == '\n':
            line_num += 1
        if state == NORMAL:
            if i + 1 < n and content[i:i+2] == '/*':
                state = IN_BLOCK_COMMENT
                i += 2
                continue
            elif i + 1 < n and content[i:i+2] == '//':
                violations.append((line_num, "cpp-comment", "C++ style '//' comment forbidden in C89"))
                while i < n and content[i] != '\n':
                    i += 1
                continue
            elif ch == '"':
                state = IN_STRING
            elif ch == "'":
                state = IN_CHAR
        elif state == IN_STRING:
            if ch == '\\':
                i += 2
                continue
            elif ch == '"':
                state = NORMAL
        elif state == IN_CHAR:
            if ch == '\\':
                i += 2
                continue
            elif ch == "'":
                state = NORMAL
        elif state == IN_BLOCK_COMMENT:
            if i + 1 < n and content[i:i+2] == '*/':
                state = NORMAL
                i += 2
                continue
        i += 1

    # 3. Trailing comma in enum
    for match in re.finditer(r'enum\s*[\w\s]*\{([^}]+)\}', content):
        enum_body = match.group(1)
        # Strip comments
        clean_body = re.sub(r'/\*.*?\*/', '', enum_body, flags=re.DOTALL)
        clean_body = re.sub(r'//.*', '', clean_body)
        if clean_body.strip().endswith(','):
            # approximate line
            start_pos = match.start()
            enum_line = content[:start_pos].count('\n') + 1
            violations.append((enum_line, "enum-trailing-comma", "Trailing comma in enum definition violates ISO C90"))

    # 4. long long check
    lines = content.splitlines()
    for l_idx, line in enumerate(lines):
        if "long long" in line:
            violations.append((l_idx + 1, "long-long", "Type 'long long' is not supported in C89"))

    return violations


def apply_mechanical_fixes(root_dir, files, verbose=False):
    """Applies automatic mechanical C89 fixes to files."""
    fixed_count = 0
    for rel in files:
        full_path = os.path.join(root_dir, rel)
        with open(full_path, "r", encoding="latin-1") as f:
            original = f.read()

        updated = original

        # 1. Convert C++ comments
        updated = convert_cpp_comments(updated)

        # 2. Fix enum trailing commas
        updated = remove_enum_trailing_commas(updated)

        # 3. Fix EOF newline
        if not updated.endswith("\n"):
            updated += "\n"

        if updated != original:
            with open(full_path, "w", encoding="latin-1") as f:
                f.write(updated)
            fixed_count += 1
            if verbose:
                print(f"  Fixed mechanical C89 issues in: {rel}")

    return fixed_count


def parse_gcc_output(output):
    """Parses GCC compiler diagnostic output."""
    errors = []
    warnings = []
    # Pattern: file:line:col: (error|warning|aviso|nota): message
    pattern = re.compile(r'^([^:\n]+):(\d+):(?:\d+:)?\s*(error|warning|aviso|fatal error):\s*(.*)$', re.MULTILINE | re.IGNORECASE)
    for match in pattern.finditer(output):
        fpath, line, kind, msg = match.groups()
        kind_lower = kind.lower()
        item = {
            "file": fpath.strip(),
            "line": int(line),
            "kind": "error" if "error" in kind_lower else "warning",
            "message": msg.strip()
        }
        if item["kind"] == "error":
            errors.append(item)
        else:
            warnings.append(item)
    return errors, warnings


def parse_watcom_output(output):
    """Parses OpenWatcom compiler diagnostic output."""
    errors = []
    warnings = []
    # Pattern: file(line): (Error!|Warning!) code: message
    pattern = re.compile(r'^([^\(\n]+)\((\d+)\):\s*(Error!|Warning!)\s*([WEN\d]+)?:\s*(.*)$', re.MULTILINE)
    for match in pattern.finditer(output):
        fpath, line, kind, code, msg = match.groups()
        item = {
            "file": fpath.strip(),
            "line": int(line),
            "kind": "error" if kind == "Error!" else "warning",
            "code": code or "",
            "message": msg.strip()
        }
        if item["kind"] == "error":
            errors.append(item)
        else:
            warnings.append(item)
    return errors, warnings


def run_target_linux(root_dir, verbose=False):
    """Builds the Linux target using make -f makefile.linux."""
    cmd = ["make", "-f", "makefile.linux"]
    print(f"{COLOR_CYAN}--> Building Linux target (GCC C89)...{COLOR_RESET}")
    res = subprocess.run(cmd, cwd=root_dir, capture_output=True, text=True)
    full_output = (res.stdout or "") + "\n" + (res.stderr or "")
    errors, warnings = parse_gcc_output(full_output)
    success = (res.returncode == 0 and len(errors) == 0)
    return success, errors, warnings, full_output


def run_target_watcom(root_dir, verbose=False):
    """Builds the Watcom DOS target using wmake."""
    makefile_name = "makefile.watcom-linux" if os.path.exists(os.path.join(root_dir, "makefile.watcom-linux")) else "makefile"
    cmd = ["wmake", "-f", makefile_name, "build"]
    print(f"{COLOR_CYAN}--> Building Watcom DOS target (OpenWatcom wcc386 C89)...{COLOR_RESET}")
    res = subprocess.run(cmd, cwd=root_dir, capture_output=True, text=True)
    full_output = (res.stdout or "") + "\n" + (res.stderr or "")
    errors, warnings = parse_watcom_output(full_output)
    success = (res.returncode == 0 and len(errors) == 0)
    return success, errors, warnings, full_output


def main():
    parser = argparse.ArgumentParser(description="DCode IDE C89 Checker & Multi-Target Builder")
    parser.add_argument("--target", choices=["all", "linux", "dos"], default="all", help="Target to build")
    parser.add_argument("--scan", action="store_true", help="Perform static C89 scan only without running makefiles")
    parser.add_argument("--fix-mechanical", action="store_true", help="Auto-fix mechanical C89 issues (comments, enums, EOF)")
    parser.add_argument("--clean", action="store_true", help="Clean artifacts before build")
    parser.add_argument("--verbose", action="store_true", help="Show full compiler output")
    args = parser.parse_args()

    root_dir = find_repo_root()
    files = get_source_files(root_dir)

    print(f"{COLOR_BOLD}======================================================{COLOR_RESET}")
    print(f"{COLOR_BOLD}   DCode IDE C89 Conformance & Build Checker{COLOR_RESET}")
    print(f"{COLOR_BOLD}======================================================{COLOR_RESET}")
    print(f"Repository Root: {root_dir}")
    print(f"Source Files:    {len(files)} (.c / .h)\n")

    if args.fix_mechanical:
        print(f"{COLOR_CYAN}Applying mechanical C89 fixes...{COLOR_RESET}")
        count = apply_mechanical_fixes(root_dir, files, verbose=True)
        print(f"{COLOR_GREEN}Fixed {count} files with mechanical C89 formatting issues.{COLOR_RESET}\n")

    if args.scan:
        print(f"{COLOR_CYAN}Running static C89 conformance scan...{COLOR_RESET}")
        total_violations = 0
        for rel in files:
            violations = scan_file(rel, os.path.join(root_dir, rel))
            if violations:
                print(f"\n{COLOR_BOLD}{rel}{COLOR_RESET}:")
                for line, code, msg in violations:
                    prefix = f"  line {line}:" if line > 0 else "  EOF:"
                    print(f"  {prefix} [{code}] {msg}")
                    total_violations += 1
        if total_violations == 0:
            print(f"\n{COLOR_GREEN}[PASS] No static C89 violations found.{COLOR_RESET}")
            return 0
        else:
            print(f"\n{COLOR_RED}[FAIL] Found {total_violations} static C89 violation(s).{COLOR_RESET}")
            return 1

    targets_to_run = []
    if args.target in ("all", "linux"):
        targets_to_run.append("linux")
    if args.target in ("all", "dos"):
        targets_to_run.append("dos")

    overall_success = True
    all_errors = 0
    all_warnings = 0

    for target in targets_to_run:
        if target == "linux":
            success, errors, warnings, raw_out = run_target_linux(root_dir, args.verbose)
        else:
            success, errors, warnings, raw_out = run_target_watcom(root_dir, args.verbose)

        all_errors += len(errors)
        all_warnings += len(warnings)

        if not success:
            overall_success = False

        status_str = f"{COLOR_GREEN}[SUCCESS]{COLOR_RESET}" if success else f"{COLOR_RED}[FAILED]{COLOR_RESET}"
        print(f"Target {target.upper()}: {status_str} - {len(errors)} error(s), {len(warnings)} warning(s)")

        if errors:
            print(f"\n{COLOR_RED}Errors for {target.upper()}:{COLOR_RESET}")
            for err in errors[:20]:
                code_str = f" [{err['code']}]" if "code" in err and err["code"] else ""
                print(f"  {err['file']}:{err['line']}{code_str}: {err['message']}")
            if len(errors) > 20:
                print(f"  ... and {len(errors) - 20} more errors.")

        if warnings:
            print(f"\n{COLOR_YELLOW}Warnings for {target.upper()}:{COLOR_RESET}")
            for warn in warnings[:20]:
                code_str = f" [{warn['code']}]" if "code" in warn and warn["code"] else ""
                print(f"  {warn['file']}:{warn['line']}{code_str}: {warn['message']}")
            if len(warnings) > 20:
                print(f"  ... and {len(warnings) - 20} more warnings.")

        if args.verbose or (not success and not errors and not warnings):
            print(f"\n--- Compiler Raw Output ({target.upper()}) ---")
            print(raw_out[-2000:] if len(raw_out) > 2000 else raw_out)
        print()

    print(f"{COLOR_BOLD}======================================================{COLOR_RESET}")
    if overall_success and all_warnings == 0:
        print(f"{COLOR_GREEN}{COLOR_BOLD}ALL TARGETS COMPILED CLEANLY UNDER C89! (0 errors, 0 warnings){COLOR_RESET}")
        return 0
    elif overall_success:
        print(f"{COLOR_YELLOW}{COLOR_BOLD}Build passed, but with {all_warnings} warning(s).{COLOR_RESET}")
        return 2
    else:
        print(f"{COLOR_RED}{COLOR_BOLD}BUILD FAILED: {all_errors} error(s), {all_warnings} warning(s).{COLOR_RESET}")
        return 1


if __name__ == "__main__":
    sys.exit(main())

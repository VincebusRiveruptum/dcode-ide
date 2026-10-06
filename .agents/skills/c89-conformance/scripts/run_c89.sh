#!/usr/bin/env bash
# run_c89.sh - Quick runner for C89 Conformance Check and Multi-Target Build
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PYTHON_EXEC="python3"

if ! command -v "$PYTHON_EXEC" &> /dev/null; then
    echo "Error: python3 is required to run the C89 checker."
    exit 1
fi

"$PYTHON_EXEC" "$SCRIPT_DIR/check_c89.py" "$@"

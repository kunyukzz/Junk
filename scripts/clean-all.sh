#!/bin/bash
# clean everything

echo "Cleaning everything..."

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

# engine
make -f "Makefile.engine" clean
if [ $? -ne 0 ]; then
    echo "Error during engine clean: $?"
    exit 1
fi

# testbed
make -f "Makefile.testbed" clean
if [ $? -ne 0 ]; then
    echo "Error during testbed clean: $?"
    exit 1
fi

echo "All assemblies cleaned successfully."

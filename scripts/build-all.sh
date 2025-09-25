#!/bin/bash
# Build script for rebuilding everything

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_ROOT"

MODE=${1:-debug}

echo -e "\033[1;34m==> Build Junk Engine in '$MODE' mode...\033[0m"

# engine
make -f Makefile.engine MODE=$MODE
echo -e "\033[1;32m✓ Engine build done\033[0m"
echo

# testbed
make -f Makefile.testbed
echo -e "\033[1;32m✓ Testbed build done\033[0m"
echo

echo -e "\033[1;32mAll assemblies built successfully.\033[0m"

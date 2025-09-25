#!/bin/bash

echo "Running Application..."

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

cd "$SCRIPT_DIR/.."
./bin/testbed

echo "Stop Application."

#!/usr/bin/env bash
set -eu

simulator="$1"
trace="$2"
output=$("$simulator" --replay "$trace")

actual=$(printf '%s\n' "$output" | sed -n -e '/^First fault/p' -e '/^State:/p')
expected='First fault at line: 4: temp 95
State: Idle, Temperature: 40
State: Fault, Temperature: -5'

if [[ "$actual" != "$expected" ]]; then
    printf 'Expected:\n%s\n\nGot:\n%s\n' "$expected" "$actual"
    exit 1
fi
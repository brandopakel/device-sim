#!/usr/bin/env bash
set -eu

simulator="$1"
recording=$(mktemp)
trap 'rm -f "$recording"' EXIT

fail() {
    printf '%s\n\nRecording:\n' "$1"
    cat "$recording"
    exit 1
}

# "temp abc" is malformed and shouldn't be saved. The empty line is on purpose.
printf 'start\n\ntemp abc\ntemp 95\nstatus\nquit\n' | "$simulator" --record "$recording" > /dev/null

header=$(head -n 1 "$recording")
[[ "$header" == "device-sim-trace 2" ]] || fail "Expected header: device-sim-trace 2"

if tail -n +2 "$recording" | grep -qvE '^[0-9]+ [^ ]'; then
    fail "Every line after the header should be: milliseconds, a space, then a command"
fi

output=$("$simulator" --replay "$recording") || fail "Replaying the recording failed"
printf '%s\n' "$output" | grep -qE '^First fault at line: 3 \(t=[0-9]+ ms\): temp 95$' \
    || fail "Expected: First fault at line: 3 (t=<ms> ms): temp 95"
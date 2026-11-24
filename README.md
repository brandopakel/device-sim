# Device Simulator

A C++20 command-line device simulator with Idle, Running, and Fault states.
The device starts Idle at 20 degrees Celsius. Unsafe temperature readings
are retained, and a safe reading does not automatically clear a fault.

## Build and run

Requires CMake 4.3+ and a C++20 compiler. Run these commands from the
repository root. Use Debug to enable the assertions in the model tests.

```sh
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
./cmake-build-debug/device_sim
```

These commands use the same build-directory name as CLion's default Debug
profile. If you choose another build directory, use it consistently for
building, running, and testing.

The executable waits for commands on standard input. Type one command per
line; use `quit` or end-of-input to exit.

## Commands

| Command | Behavior |
| --- | --- |
| `start` | Changes Idle to Running; rejects other states. |
| `stop` | Changes Running to Idle; rejects other states. |
| `status` | Displays the current state and latest temperature. |
| `temp <integer>` | Stores a reading. Values outside 0-80 inclusive cause Fault; safe readings preserve the current state. |
| `reset` | Changes Fault to Idle only when the latest reading is safe. |
| `quit` | Exits the program. |

`temp` requires exactly one integer representable by C++ `int`. Missing
values, nonnumeric values, numeric suffixes, overflow, and extra arguments
are rejected without changing the device. All other commands take no
arguments. Blank lines are ignored, and surrounding whitespace is allowed.

## Fault recovery example

Enter the following after launching the simulator:

```text
start
temp 95
status
reset
temp 40
status
reset
status
quit
```

The reading of 95 puts the device in Fault, and the first reset is rejected.
Cooling to 40 preserves Fault. The second reset succeeds, leaving the device
Idle at 40.

## Run tests

After building, run the registered tests:

```sh
ctest --test-dir cmake-build-debug --output-on-failure
```

CTest runs four tests: `device_model` (the assertions in `tests.cpp`),
`replay_two_faults` (a version 1 trace), `replay_timestamps` (a version 2
trace) and `record_timestamps` (records a short session, then replays it).
To see the assertion executable's `All tests passed!` message directly, run:

```sh
./cmake-build-debug/device_sim_tests
```

The model tests cover state transitions, rejected commands, recovery,
retention of unsafe readings, and temperature boundaries. `main.cpp` contains
the interactive CLI, `device.h` and `device.cpp` define the model, and
`tests.cpp` contains the assertions. The `test_*.sh` scripts and `trace-*.txt`
files drive the replay and recording tests.

## Recording and replay

Run these commands from the repository root:

```sh
./cmake-build-debug/device_sim --record recorded.txt
./cmake-build-debug/device_sim --replay recorded.txt
```

Recording executes commands and displays their output while saving valid
commands to the file. The output file is overwritten when recording starts.
Malformed commands are reported and omitted; valid commands that cause
Fault or are rejected by the device's current state are still recorded.

Recording writes version 2 traces: a `device-sim-trace 2` header, then one
command per line, each starting with the milliseconds since recording began.
Blank lines are not recorded.

```text
device-sim-trace 2
0 start
1200 temp 30
2450 temp 95
```

Replay accepts version 1 traces (`device-sim-trace 1`, commands without
timestamps) and version 2 traces. It starts with a fresh device, executes
commands in file order, stops at `quit` or end-of-file, and reports the first
command that puts the device in Fault:

```text
First fault at line: 4 (t=2450 ms): temp 95
```

Version 1 traces print the same line without the `(t=... ms)` part. Line
numbers count the header as line 1. Any other header stops replay with exit
code 1. So does a malformed command, or a version 2 timestamp that is missing,
negative or earlier than the line before; those errors include the line
number.

Running without arguments starts an ordinary interactive session.
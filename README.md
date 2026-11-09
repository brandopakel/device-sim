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

After building, run the registered model test separately from the CLI:

```sh
ctest --test-dir cmake-build-debug --output-on-failure
```

CTest should report `device_model` passing. To see the assertion executable's
`All tests passed!` message directly, run:

```sh
./cmake-build-debug/device_sim_tests
```

The model tests cover state transitions, rejected commands, recovery,
retention of unsafe readings, and temperature boundaries. `main.cpp` contains
the interactive CLI, `device.h` and `device.cpp` define the model, and
`tests.cpp` contains the assertions.

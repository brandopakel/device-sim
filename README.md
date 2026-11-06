# Device Simulator

A C++20 device simulator with Idle, Running, and Fault states.

Supports start/stop commands, temperature readings, fault detection,
and explicit recovery. Unsafe readings are retained, and a safe
reading does not automatically clear a fault.

Assertions cover state transitions, rejected commands, recovery,
and temperature boundaries.

## Build and run

Requires CMake 4.3+ and a C++20 compiler. Use Debug to enable assertions.

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build
    ./build/device_sim

Expected output: All tests passed!

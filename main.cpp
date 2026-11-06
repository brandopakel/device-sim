#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>

enum class State {
    Idle,
    Running,
    Fault
};

struct Device {
    State state = State::Idle;
    int temperature = 20;
};

bool isSafeTemperature(int temperature) {
    return temperature>=0 && temperature<=80;
}

bool start(Device& device) {
    if (device.state != State::Idle) {
        return false;
    }
    device.state = State::Running;
    return true;
}

bool stop(Device& device) {
    if (device.state != State::Running) {
        return false;
    }
    device.state = State::Idle;
    return true;
}

bool temperatureUpdate(Device& device, int temperature) {
    // Always retain the last reading
    device.temperature = temperature;

    if (!isSafeTemperature(temperature)) {
        device.state = State::Fault;
        return false;
    }

    // A safe reading does not automatically clear an existing fault.
    return true;
}

bool reset(Device& device) {
    if (device.state != State::Fault || !isSafeTemperature(device.temperature)){
        return false;
    }
    device.state = State::Idle;
    return true;
}

int main() {
    Device device;

    // Initial state.
    assert(device.state == State::Idle);
    assert(device.temperature == 20);

    // Rejected commands leave an idle device unchanged.
    assert(!stop(device));
    assert(device.state == State::Idle);
    assert(device.temperature == 20);

    assert(!reset(device));
    assert(device.state == State::Idle);
    assert(device.temperature == 20);

    // Start successfully, then reject a repeated start.
    assert(start(device));
    assert(device.state == State::Running);

    assert(!start(device));
    assert(device.state == State::Running);
    assert(device.temperature == 20);

    // A safe reading preserves normal operation.
    assert(temperatureUpdate(device, 50));
    assert(device.temperature == 50);
    assert(device.state == State::Running);

    // An unsafe reading is stored and causes a fault.
    assert(!temperatureUpdate(device, 95));
    assert(device.temperature == 95);
    assert(device.state == State::Fault);

    // Commands cannot bypass the fault.
    assert(!start(device));
    assert(device.state == State::Fault);
    assert(device.temperature == 95);

    assert(!stop(device));
    assert(device.state == State::Fault);
    assert(device.temperature == 95);

    assert(!reset(device));
    assert(device.state == State::Fault);
    assert(device.temperature == 95);

    // Cooling down does not automatically clear the fault.
    assert(temperatureUpdate(device, 40));
    assert(device.temperature == 40);
    assert(device.state == State::Fault);

    // Explicit reset allows operation again.
    assert(reset(device));
    assert(device.state == State::Idle);
    assert(device.temperature == 40);

    assert(start(device));
    assert(device.state == State::Running);

    assert(stop(device));
    assert(device.state == State::Idle);
    assert(device.temperature == 40);

    // Check temperature boundaries from every starting state.
    for (State initialState :
         {State::Idle, State::Running, State::Fault}) {

        // Both endpoints are valid and preserve the current state.
        for (int temperature : {0, 80}) {
            Device boundaryDevice{initialState, 20};

            assert(temperatureUpdate(boundaryDevice, temperature));
            assert(boundaryDevice.temperature == temperature);
            assert(boundaryDevice.state == initialState);
        }

        // Values just outside the range cause a fault.
        for (int temperature : {-1, 81}) {
            Device boundaryDevice{initialState, 20};

            assert(!temperatureUpdate(boundaryDevice, temperature));
            assert(boundaryDevice.temperature == temperature);
            assert(boundaryDevice.state == State::Fault);
        }
    }

    std::cout << "All tests passed!\n";
}
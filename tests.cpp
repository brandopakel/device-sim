#include "device.h"
#include <cassert>
#include <initializer_list>
#include <iostream>

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
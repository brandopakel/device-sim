#include "device.h"

bool isSafeTemperature(const int temperature) {
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

bool temperatureUpdate(Device& device, const int temperature) {
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
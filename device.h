#pragma once

enum class State {
    Idle,
    Running,
    Fault
};

struct Device {
    State state = State::Idle;
    int temperature = 20;
};

bool start(Device& device);
bool stop(Device& device);
bool reset(Device& device);
bool isSafeTemperature(int temperature);
bool temperatureUpdate(Device& device, int temperature);
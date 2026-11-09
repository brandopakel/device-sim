#include "device.h"
#include <iostream>
#include <string>
#include <sstream>

std::string stateName(State state) {
    switch (state) {
        case State::Idle:
            return "Idle";
        case State::Running:
            return "Running";
        case State::Fault:
            return "Fault";
        default:
            return "Unknown";
    }
}

int main() {
    Device device;
    std::string line;

    while (std::getline(std::cin, line)) {
        std::istringstream input(line);
        std::string command;

        if (!(input >> command)) {
            continue;
        }
        const bool takesNoArguments =
            command == "start" ||
            command == "stop" ||
            command == "status" ||
            command == "reset" ||
            command == "quit";
        if (takesNoArguments) {
            std::string extra;
            if (input >> extra) {
                std::cout << "Command rejected\n";
                continue;
            }
        }

        if (command == "quit") {
            break;
        } else if (command == "start") {
            if (!start(device)) {
                std::cout << "Device failed to start \n";
            } else {
                std::cout << "Device successfully started \n";
            }
        } else if (command == "stop") {
            if (!stop(device)) {
                std::cout << "Device failed to stop \n";
            } else {
                std::cout << "Device successfully stopped \n";
            }
        } else if (command == "status"){
            std::cout << "State: " << stateName(device.state) << ", Temperature: " << device.temperature << std::endl;
        } else if (command == "temp") {
            int temperature;
            std::string extra;
            if (!(input >> temperature)) {
                std::cout << "Error reading temperature \n";
                continue;
            }
            if (input >> extra) {
                std::cout << "Command rejected\n";
                continue;
            }
            if (!temperatureUpdate(device, temperature)){
                std::cout << "Reading stored: unsafe temperature; device is in Fault.\n";
            } else {
                std::cout << "Temperature recorded: safe range\n";
            }
        } else if (command == "reset") {
            if (!reset(device)) {
                std::cout << "Device failed to reset \n";
            } else {
                std::cout << "Device successfully reset \n";
            }
        } else {
            std::cout << "Unknown command \n";
        }
    }
    return 0;
}


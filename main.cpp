#include "device.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstddef>

enum class CommandResult {
    Continue,
    Quit,
    Invalid
};

std::string stateName(State state);
CommandResult processCommand(Device& device, const std::string& line) {
        std::istringstream input(line);
        std::string command;

        if (!(input >> command)) {
            return CommandResult::Continue;
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
                return CommandResult::Invalid;
            }
        }

        if (command == "quit") {
            return CommandResult::Quit;
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
                return CommandResult::Invalid;
            }
            if (input >> extra) {
                std::cout << "Command rejected\n";
                return CommandResult::Invalid;
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
            return CommandResult::Invalid;
        }
    return CommandResult::Continue;
}

bool replayCommands(Device& device, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open trace" << filename << std::endl;
        return false;
    }

    std::string line;
    if (!std::getline(file,line)) {
        std::cerr << "Could not read trace header";
        return false;
    }
    if (line != "device-sim-trace 1") {
        std::cerr << "Invalid trace header";
        return false;
    }

    std::size_t lineNumber = 1;
    while (std::getline(file,line)) {
        ++lineNumber;
        CommandResult result = processCommand(device, line);
        if (result == CommandResult::Invalid) {
            std::cerr << "Invalid command at line " << lineNumber << "\n";
            return false;
        }
        if (result == CommandResult::Quit) {
            return true;
        }
    }

    if (file.bad()) {
        std::cerr << "Error reading trace file: " << filename << std::endl;
        return false;
    }

    return true;
}

bool recordCommands(Device& device, const std::string& filename) {
    std::ofstream file(filename);

    if (!file) {
        std::cerr << "Could not open recording" << filename << std::endl;
        return false;
    }

    file << "device-sim-trace 1\n";

    if (!file) {
        std::cerr << "Could not write trace header\n";
        return false;
    }

    std::string line;
    while (std::getline(std::cin,line)) {
        CommandResult result = processCommand(device, line);
        if (result == CommandResult::Invalid) {
            continue;
        }
        file << line << "\n";
        if (!file) {
            std::cerr << "Error writing recording\n";
            return false;
        }
        if (result == CommandResult::Quit) {
            break;
        }
    }

    if (std::cin.bad()) {
        std::cerr << "Error reading interactive input\n";
        return false;
    }

    file.close();

    if (!file) {
        std::cerr << "Error finishing recording\n";
        return false;
    }

    return true;
}

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

int main(int argc, char* argv[]) {
    Device device;

    if (argc==1) {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (processCommand(device, line) == CommandResult::Quit) {
                break;
            }
        }

        if (std::cin.bad()) {
            std::cerr << "Error reading interactive input\n";
            return 1;
        }

        return 0;
    }

    if (argc==3) {
        std::string mode = argv[1];
        if (mode == "--record") {
            return recordCommands(device, argv[2]) ? 0 : 1;
        }
        if (mode == "--replay") {
            return replayCommands(device, argv[2]) ? 0 : 1;
        }
    }

    std::cerr << "Usage:\n"
            << " " << argv[0] << "\n"
            << " " << argv[0] << " --record <file>\n"
            << " " << argv[0] << " --replay <file>\n";
    return 1;
}

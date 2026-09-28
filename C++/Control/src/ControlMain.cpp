#include "control/ControlLayer.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        control::ControlLayer control;

        while (true) {
            control.controlPipelineStep();
        }
    }
    catch (const std::exception& exception) {
        std::cerr
            << "Control error: "
            << exception.what()
            << '\n';

        return 1;
    }
}

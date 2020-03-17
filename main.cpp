#include "test_keys.hpp"
#include "input_to_control.hpp"

#include <iostream>
#include <iomanip>
#include <termcap.h>

int main()
{
    using namespace TerminalControl;

    TerminalController controller;

    controller.setControlBitness(ControlBits::S7C1T);
    //controller.switchToStandardCharacterSet();
    controller.switchToAlternateCharacterSet();
    controller.changeCharacterSet(CharacterSet::UTF8, CountryEncoding::German);
    controller.printReport({ReportCommands::KeyboardStatus});

    controller.setPrivateMode({1039});

    key_print_loop();

    return 0;
}

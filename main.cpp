#include "test_keys.hpp"
#include "input_to_control.hpp"

#include <iostream>
#include <iomanip>

int main()
{
    using namespace TerminalControl;

    //setControlBitness(ControlBits::S8C1T);

    changeCharacterSet(CharacterSet::UTF8, CountryEncoding::German);

    key_print_loop();

    return 0;
}

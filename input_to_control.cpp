#include "input_to_control.hpp"

namespace TerminalControl
{
//#####################################################################################################################
    std::string inputToSequence(Input const& input, TerminalMode mode)
    {
        auto [code, ctrl, alt, shift] = input;

        bool isWithin = [&code](int lower, int higher)
        {
            return code >= lower && code <= higher;
        };

        int modifier = 0;
        if (shift && !alt && !ctrl)
            modifier = 2;
        else if (alt && !shift && !ctrl)
            modifier = 3;
        else if (shift && alt && !ctrl)
            modifier = 4;
        else if (ctrl && !shift && !alt)
            modifier = 5;
        else if (ctrl && shift && !alt)
            modifier = 6;
        else if (ctrl && alt && !shift)
            modifier = 7;
        else if (ctrl && alt && shift)
            modifier = 8;

        // letters
        if (isWithin(65, 90))
        {
            // ctrl
            if (modifier == 5)
                return {static_cast <char> (code - 64)};

            // alt
            if (modifier == 3)
                return {'\x1b', static_cast <char> (code - 65) + 'a'};
        }
    }
//---------------------------------------------------------------------------------------------------------------------
//#####################################################################################################################
}

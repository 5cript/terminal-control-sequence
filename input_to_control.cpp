#include "input_to_control.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

namespace TerminalControl
{
//#####################################################################################################################
    namespace
    {
        std::string csi(ControlBits bits)
        {
            if (bits == ControlBits::S7C1T)
                return {'\x1b', '['};
            else
                return {'\x9b'};
        }

        #define CSI csi(bits)
        #define S std::string

        /**
         * @brief applicationKeypadNumbers
         * @param number '0' to '9'
         * @return A sequence interpreted by the terminal.
         */
        std::string applicationKeypadNumbers(char number, ControlBits bits)
        {
            switch(number)
            {
                case('0'): return csi(bits) + S{'2', '~'};
                case('1'): return csi(bits) + S{'F'};
                case('2'): return csi(bits) + S{'B'};
                case('3'): return csi(bits) + S{'6', '~'};
                case('4'): return csi(bits) + S{'D'};
                case('5'): return csi(bits) + S{'E'};
                case('6'): return csi(bits) + S{'C'};
                case('7'): return csi(bits) + S{'H'};
                case('8'): return csi(bits) + S{'A'};
                case('9'): return csi(bits) + S{'5', '~'};
                default: return {};
            }
        }
    }
//#####################################################################################################################
    std::string inputToSequence(Input const& input, TerminalMode mode)
    {
        const auto [code, ctrl, alt, shift] = input;

        auto isWithin = [code{input.keyCode}](int lower, int higher) -> bool
        {
            return static_cast <int> (code) >= lower && static_cast <int> (code) <= higher;
        };
        auto charPulldown = [code{input.keyCode}](int pulldown) -> char
        {
            return static_cast <char> (code - pulldown);
        };
        auto charRelativeToZero = [&charPulldown](int pulldown, char offset) -> char
        {
            return static_cast <char> (charPulldown(pulldown) + offset);
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
                return {'\x1b', charRelativeToZero(65, 'a')};
        }

        // keypad numbers
        if (isWithin(97, 105))
        {
            auto c = charRelativeToZero(97, '0');
            if (mode.keypadMode == Keypad::normal)
                return {c};
            else
                return applicationKeypadNumbers(c, mode.bits);
        }

        return {};
    }
//---------------------------------------------------------------------------------------------------------------------
    std::string changeCharacterSet(CharacterSet set, CountryEncoding enc, bool returnAsString)
    {
        auto getEnc = [&]()
        {
            switch (enc)
            {
                case(CountryEncoding::_DEFAULT_CASE_IS_ERROR_IF_GSET_): throw std::runtime_error("you must supply a country encoding when using the GX character sets, X=0,1,2,3");
                case(CountryEncoding::DrawingMode): return '0';
                case(CountryEncoding::UnitedKingdom): return 'A';
                case(CountryEncoding::UnitedStates): return 'B';
                case(CountryEncoding::Dutch): return '4';
                case(CountryEncoding::Finnish): return '5';
                case(CountryEncoding::French): return 'R';
                case(CountryEncoding::FrenchCanadian): return 'Q';
                case(CountryEncoding::German): return 'K';
                case(CountryEncoding::Italian): return 'Y';
                case(CountryEncoding::NorwegianDanish): return 'E';
                case(CountryEncoding::Spanish): return 'Z';
                case(CountryEncoding::Swedish): return 'H';
                case(CountryEncoding::Swiss): return '=';
            }
            throw std::runtime_error("passed country encoding is not understood");
        };

        auto printToStream = [&](auto& stream)
        {
            switch (set)
            {
                case(CharacterSet::ISO8859_1):
                {
                    stream << "\x1b%@" << std::flush;
                    break;
                }
                case(CharacterSet::UTF8):
                {
                    stream << "\x1b%G" << std::flush;
                    break;
                }
                case(CharacterSet::G0):
                {
                    stream << "\x1b(" << getEnc() << std::flush;
                    break;
                }
                case(CharacterSet::G1):
                {
                    stream << "\x1b)" << getEnc() << std::flush;
                    break;
                }
                case(CharacterSet::G2):
                {
                    stream << "\x1b*" << getEnc() << std::flush;
                    break;
                }
                case(CharacterSet::G3):
                {
                    stream << "\x1b+" << getEnc() << std::flush;
                    break;
                }
            }
        };

        if (returnAsString)
        {
            std::stringstream sstr;
            printToStream(sstr);
            return sstr.str();
        }
        else
            printToStream(std::cout);
        return {};
    }
//---------------------------------------------------------------------------------------------------------------------
    void setControlBitness(ControlBits bits)
    {
        if (bits == ControlBits::S7C1T)
            std::cout << "\x1b F";
        else if (bits == ControlBits::S8C1T)
            std::cout << "\x1b G";
    }
//#####################################################################################################################
}

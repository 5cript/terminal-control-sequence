#include "input_to_control.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <array>

namespace TerminalControl
{
//#####################################################################################################################
    namespace
    {
#define S std::string
#define IS8 (bits == ControlBits::S8C1T)
#define IS7 (bits == ControlBits::S7C1T)

        namespace Controls
        {
            // Control Sequence Introducer
            std::string csi(ControlBits bits)
            {
                return IS7 ? S{'\x1b', '['} : S{'\x9b'};
            }
            // Index
            [[maybe_unused]] std::string ind(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'D'} : S{'\x84'};
            }
            // Next Line
            [[maybe_unused]] std::string nel(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'E'} : S{'\x85'};
            }
            // Tab Set
            [[maybe_unused]] std::string hts(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'H'} : S{'\x88'};
            }
            // Reverse Index
            [[maybe_unused]] std::string ri(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'M'} : S{'\x8d'};
            }
            // Single Shift Select G2
            [[maybe_unused]] std::string ss2(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'N'} : S{'\x8e'};
            }
            // Single Shift Select G3
            std::string ss3(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'O'} : S{'\x8f'};
            }
            // Device Control String
            [[maybe_unused]] std::string dcs(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'P'} : S{'\x90'};
            }
            // Start of Guarded Area
            [[maybe_unused]] std::string spa(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'V'} : S{'\x96'};
            }
            // End of Guarded Area
            [[maybe_unused]] std::string epa(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'W'} : S{'\x97'};
            }
            // Start of String
            [[maybe_unused]] std::string sos(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'X'} : S{'\x98'};
            }
            // Return Terminal ID
            [[maybe_unused]] std::string decid(ControlBits bits)
            {
                return IS7 ? S{'\x1b', 'Z'} : S{'\x9a'};
            }
            // String Terminator
            [[maybe_unused]] std::string st(ControlBits bits)
            {
                return IS7 ? S{'\x1b', '\\'} : S{'\x9c'};
            }
            // Operating System Command
            [[maybe_unused]] std::string osc(ControlBits bits)
            {
                return IS7 ? S{'\x1b', ']'} : S{'\x9d'};
            }
            // Privacy Message
            [[maybe_unused]] std::string pm(ControlBits bits)
            {
                return IS7 ? S{'\x1b', '^'} : S{'\x9e'};
            }
            // Application Program Command
            [[maybe_unused]] std::string apc(ControlBits bits)
            {
                return IS7 ? S{'\x1b', '_'} : S{'\x9f'};
            }
        }

#define CSI csi(bits)

        /**
         * @brief applicationKeypadNumbers
         * @param number '0' to '9'
         * @return A sequence interpreted by the terminal.
         */
        std::string applicationKeypadNumbers(char number, int scaMode, TerminalMode const& mode)
        {
            using namespace Controls;

            auto f = [&]()
            {
                if (scaMode != 0 && mode.keypadMode == Keypad::Application)
                {
                    if (number != '3' && number != '9')
                        return csi(mode.bits) + S{'1', ';'} + std::to_string(scaMode);
                    else if (number == 3)
                        return csi(mode.bits) + S{'6', ';'} + std::to_string(scaMode);
                    else if (number == 9)
                        return csi(mode.bits) + S{'5', ';'} + std::to_string(scaMode);
                }

                if (mode.decckm == DECCKM::Normal)
                    return csi(mode.bits);
                else
                    return ss3(mode.bits);
            };

            switch(number)
            {
                case('1'): return f() + S{'F'};
                case('2'): return f() + S{'B'};
                case('4'): return f() + S{'D'};
                case('5'): return f() + S{'E'};
                case('6'): return f() + S{'C'};
                case('7'): return f() + S{'H'};
                case('8'): return f() + S{'A'};

                case('0'): return csi(mode.bits) + S{'2', '~'};
                case('3'): return csi(mode.bits) + S{'6', '~'};
                case('9'): return csi(mode.bits) + S{'5', '~'};
                default: return {};
            }
        }

        std::string applicationKeypadChars(int code, TerminalMode const& mode)
        {
            using namespace Controls;

            if (mode.keypadMode == Keypad::Application)
            {
                if (code == 110)
                    return csi(mode.bits) + S{'3', '~'};
            }

            switch(code)
            {
                case(106): return {'*'};
                case(107): return {'+'};
                case(108): return {'-'}; // seperator
                case(109): return {'-'}; // minus
                case(110): return {','};
                case(111): return {'/'};
            }
            return {};
        }

        std::string shiftNumber(char number, CharacterSet cset)
        {
            switch (number)
            {
                case('0'): return {'='};
                case('1'): return {'!'};
                case('2'): return {'"'};
                case('4'): return {'$'};
                case('5'): return {'%'};
                case('6'): return {'&'};
                case('7'): return {'/'};
                case('8'): return {'('};
                case('9'): return {')'};
                default: break;
            }
            if (number == '3')
            {
                switch (cset)
                {
                    case (CharacterSet::UTF8): return {'\xc2', '\xa7'};
                    default: return {'\xa7'};
                }
            }
            return {};
        }

        std::string ctrlNumber(char number)
        {
            switch (number)
            {
                case('0'): return {'0'};
                case('1'): return {'1'};
                case('2'): return {'\x00'};
                case('3'): return {'\x1b'};
                case('4'): return {'\x1c'};
                case('5'): return {'\x1d'};
                case('6'): return {'\x1e'};
                case('7'): return {'\x1f'};
                case('8'): return {'\x7f'};
                case('9'): return {'9'};
                default: break;
            }
            return {};
        }

        template <unsigned... List>
        bool isAnyOf(unsigned code)
        {
            return ((code == List) || ...);
        }

        std::string concatenateNumbers(std::vector <int> const& nums)
        {
            if (nums.empty())
                return {};

            std::string str;
            for (auto i = std::begin(nums), end = std::end(nums); i < end - 1; ++i)
            {
                str += std::to_string(*i);
                str.push_back(';');
            }
            str += std::to_string(*std::rbegin(nums));
            return str;
        }
    }
//#####################################################################################################################
    std::string inputToSequence(Input const& input, TerminalMode mode)
    {
        const auto [code, ctrl, alt, shift, altGr] = input;

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

        using namespace Controls;

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

        std::string seqStart;
        std::string seqStartCsi = csi(mode.bits);
        if (mode.decckm == DECCKM::Application)
            seqStart = ss3(mode.bits);
        else
            seqStart = csi(mode.bits);

        if (modifier != 0)
        {
            seqStartCsi = csi(mode.bits) + "1;" + std::to_string(modifier);
            seqStart = csi(mode.bits) + "1;" + std::to_string(modifier);
        }

        // letters
        if (isWithin(65, 90))
        {
            if (modifier == 0 && altGr && charRelativeToZero(65, 'A') == 'M')
            {
                if (mode.charSet == CharacterSet::UTF8)
                    return {'\xc2', '\xb5'};
                else
                    return {'\xb5'};
            }
            if (modifier == 0 && altGr && charRelativeToZero(65, 'A') == 'Q')
            {
                return {'@'};
            }

            // ctrl
            if (modifier == 5)
                return {static_cast <char> (code - 64)};

            // shift
            if (modifier == 2)
                return {charRelativeToZero(65, 'A')};

            // alt
            if (modifier == 3)
            {
                if (mode.altSendsEscape)
                    return {'\x1b', charRelativeToZero(65, 'a')};
                else
                {
                    if (mode.charSet != CharacterSet::UTF8)
                        return {charRelativeToZero(65, '\xe1')};
                    else
                        return {'\xc3', charRelativeToZero(65, '\xa1')};
                }
            }
        }

        // numbers
        if (isWithin(48, 57))
        {
            // shift + altgr in utf8 = some chars
            if (mode.charSet == CharacterSet::UTF8)
            {
                if (modifier == 5)
                    return ctrlNumber(charRelativeToZero(48, '0'));

                if (shift && altGr)
                {
                    const std::string table[] = {
                        {'\xc2', '\xb0'}, // 0
                        {'\xc2', '\xa1'}, // 1 ...
                        {'\xe2', '\x85', '\x9b'},
                        {'\xc2', '\xa4'},
                        {'\xe2', '\x85', '\x9c'},
                        {'\xe2', '\x85', '\x9d'},
                        {'\xe2', '\x85', '\x9e'},
                        {'\xe2', '\x84', '\xa2'},
                        {'\xc2', '\xb1'}
                    };

                    return table[code - 48];
                }
                if (altGr)
                {
                    const std::string table[] = {
                        {'}'}, // 0
                        {'\xc2', '\xb9'}, // 1 ...
                        {'\xc2', '\xb2'},
                        {'\xc2', '\xb3'}, // 3
                        {'\xc2', '\xbc'},
                        {'\xc2', '\xbd'},
                        {'\xc2', '\xac'},
                        {'{'},
                        {'['},
                        {']'}
                    };

                    return table[code - 48];
                }
            }

            if (modifier != 2 && altGr)
            {
                const std::string table[] = {
                    {'}'}, // 0
                    {'\xb9'}, // 1 ...
                    {'\xb2'},
                    {'\xb3'}, // 3
                    {'\xbc'},
                    {'\xbd'},
                    {'\xac'},
                    {'{'},
                    {'['},
                    {']'}
                };

                return table[code - 48];
            }

            // shift
            if (modifier == 2)
                return shiftNumber(charRelativeToZero(48, '0'), mode.charSet);

            // ctrl
            if (modifier == 5)
                return ctrlNumber(charRelativeToZero(48, '0'));

            // default:
            return {charRelativeToZero(48, '0')};
        }

        // keypad numbers
        if (isWithin(97, 105))
        {
            auto c = charRelativeToZero(97, '0');
            if (mode.keypadMode == Keypad::Normal)
                return {c};
            else
            {
                if (shift)
                    return {c};
                else
                    return applicationKeypadNumbers(c, modifier, mode);
            }
        }

        //  arrow keys
        if (isWithin(37, 40))
        {
            if (mode.decckm == DECCKM::Normal)
            {
                if (code == 38) return seqStart + S{'A'};
                if (code == 40) return seqStart + S{'B'};
                if (code == 39) return seqStart + S{'C'};
                if (code == 37) return seqStart + S{'D'};
            }
            else if (mode.decckm == DECCKM::Application)
            {
                if (code == 38) return seqStart + S{'A'};
                if (code == 40) return seqStart + S{'B'};
                if (code == 39) return seqStart + S{'C'};
                if (code == 37) return seqStart + S{'D'};
            }
        }

        // home
        if (code == 36)
        {
            seqStart + S{'H'};
        }

        // end
        if (code == 35)
        {
            seqStart + S{'F'};
        }

        // insert
        if (code == 45)
            return seqStartCsi + S{'2', '~'};

        // delete
        if (code == 46)
            return seqStartCsi + S{'3', '~'};

        // page up
        if (code == 33)
            return seqStartCsi + S{'5', '~'};

        // page down
        if (code == 34)
            return seqStartCsi + S{'6', '~'};

        // keypad math and comma
        if (isWithin(106, 111))
        {
            return applicationKeypadChars(code, mode);
        }

        if (code == 9 && shift)
            return csi(mode.bits) + S{'Z'};

        if (code == 220 && shift)
            return {'\xc2', '\xb0'};

        /*
         *  + keybd: 187
         *  # keybd: 191
         *  - keybd: 189
         *  . keybd: 190
         *  , keybd: 188
         *  < keybd:
         */
        if (code == 187)
        {
            // needs more code page support

            if (shift) return {'*'};
            if (altGr) return {'~'};
            return {'+'};
        }
        if (code == 191)
        {
            // needs more code page support

            if (shift) return {'\''};
            return {'#'};
        }
        if (code == 189)
        {
            // needs more code page support

            if (shift) return {'_'};
            return {'-'};
        }
        if (code == 190)
        {
            // needs more code page support

            if (shift) return {':'};
            return {'.'};
        }
        if (code == 188)
        {
            // needs more code page support

            if (shift) return {';'};
            return {','};
        }

        if (code == 220)
        {
            if (shift)
            {
                if (mode.charSet == CharacterSet::UTF8)
                    return {'\xc2', '\xb0'};
                else
                    return {'\xb0'};
            }
            return {'^'};
        }

        // oe
        if (code == 192)
        {
            if (shift)
            {
                if (mode.charSet == CharacterSet::UTF8)
                    return {'\xc3', '\x96'};
                else
                    return {'\xd6'};
            }
            if (mode.charSet == CharacterSet::UTF8)
                return {'\xc3', '\xb6'};
            else
                return {'\xf6'};
        }

        // ae
        if (code == 222)
        {
            if (shift)
            {
                if (mode.charSet == CharacterSet::UTF8)
                    return {'\xc3', '\x84'};
                else
                    return {'\xc4'};
            }
            if (mode.charSet == CharacterSet::UTF8)
                return {'\xc3', '\xa4'};
            else
                return {'\xe4'};
        }

        // ue
        if (code == 192)
        {
            if (shift)
            {
                if (mode.charSet == CharacterSet::UTF8)
                    return {'\xc3', '\x9c'};
                else
                    return {'\xdc'};
            }
            if (mode.charSet == CharacterSet::UTF8)
                return {'\xc3', '\xbc'};
            else
                return {'\xfc'};
        }

        // backspace
        if (isAnyOf <
            7, 8, 9, 13, 0x20
        >(code))
            return {static_cast <char> (code)};

        // for unknowns
        return {static_cast <char> (code)};
    }
//#####################################################################################################################
    std::string TerminalController::changeCharacterSet(CharacterSet set, CountryEncoding enc, bool returnAsString)
    {
        auto getEnc = [&]() -> std::string
        {
            switch (enc)
            {
                case(CountryEncoding::_DEFAULT_CASE_IS_ERROR_IF_GSET_): throw std::runtime_error("you must supply a country encoding when using the GX character sets, X=0,1,2,3");
                case(CountryEncoding::DrawingMode): return {'0'};
                case(CountryEncoding::UnitedKingdom): return {'A'};
                case(CountryEncoding::UnitedStates): return {'B'};
                case(CountryEncoding::Dutch): return {'4'};
                case(CountryEncoding::Finnish): return {'5'};
                case(CountryEncoding::French): return {'R'};
                case(CountryEncoding::FrenchCanadian): return {'Q'};
                case(CountryEncoding::German): return {'K'};
                case(CountryEncoding::Italian): return {'Y'};
                case(CountryEncoding::NorwegianDanish): return {'E'};
                case(CountryEncoding::Spanish): return {'Z'};
                case(CountryEncoding::Swedish): return {'H'};
                case(CountryEncoding::Greek): return {'"'};
                case(CountryEncoding::Swiss): return {'='};
                case(CountryEncoding::Turkish): return "%2";
                case(CountryEncoding::Portuguese): return "%6";
                case(CountryEncoding::Hebrew): return "%=";
                case(CountryEncoding::Supplemental): return {'<'};
                case(CountryEncoding::Technical): return {'>'};
                case(CountryEncoding::Russian): return "&5";
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
    void TerminalController::switchToStandardCharacterSet()
    {
        std::cout << "\x0f" << std::flush;
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::switchToAlternateCharacterSet()
    {
        std::cout << "\x0e" << std::flush;
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::setControlBitness(ControlBits bits)
    {
        if (bits == ControlBits::S7C1T)
            std::cout << "\x1b F" << std::flush;
        else if (bits == ControlBits::S8C1T)
            std::cout << "\x1b G" << std::flush;
        bits_ = bits;
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::getTerminalId()
    {
        using namespace Controls;
        std::cout << (csi(bits_) + S{'c'}) << std::flush;
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::lockKeyboard(bool lock)
    {
        if (lock)
            setMode({2});
        else
            setMode({2}, true);
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::setMode(std::vector <int> const& modeParams, bool set)
    {
        using namespace Controls;

        std::string s = csi(bits_);
        s += concatenateNumbers(modeParams);
        if (set)
            s += "h";
        else
            s += "l";

        std::cout << s << std::flush;
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::setIrMode(IRMode mode)
    {
        if (mode == IRMode::Insert)
            setMode({4});
        else
            setMode({4}, false);
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::setPrivateMode(std::vector <int> const& modeParams, bool set)
    {
        using namespace Controls;

        std::string s = csi(bits_);
        s += "?";
        s += concatenateNumbers(modeParams);
        if (set)
            s += "h";
        else
            s += "l";

        std::cout << s << std::flush;
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::printReport(std::vector <int> const& params)
    {
        using namespace Controls;

        std::string s = csi(bits_);
        s += "?";
        s += concatenateNumbers(params);
        s += "n";

        std::cout << s << std::flush;
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::printReport(std::vector <ReportCommands> const& params)
    {
        std::vector <int> v;
        for (auto const& p: params)
            v.push_back(static_cast <int> (p));
        printReport(v);
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::enableMouseMode()
    {
        setPrivateMode({1000});
    }
//---------------------------------------------------------------------------------------------------------------------
    void TerminalController::enableFocusDectection()
    {
        setPrivateMode({1004});
    }
//#####################################################################################################################
}

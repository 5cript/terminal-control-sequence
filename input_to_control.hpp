#pragma once

#include <string>
#include <vector>

// https://www.xfree86.org/current/ctlseqs.html

namespace TerminalControl
{
    enum class ControlBits
    {
        S7C1T = 7,
        S8C1T = 8
    };

    enum class Keypad
    {
        Normal = 0, // DECPNM
        Application = 1 // DECPAM
    };

    enum class CharacterSet
    {
        ISO8859_1 = 0,
        UTF8 = 1,
        G0 = 2,
        G1 = 3,
        G2 = 4,
        G3 = 5
    };

    enum class CountryEncoding
    {
        _DEFAULT_CASE_IS_ERROR_IF_GSET_,
        DrawingMode = 1, // 0, DEC Special Character and Line Drawing Set
        UnitedKingdom = 2,
        UnitedStates = 3,
        Dutch = 4,
        Finnish = 5,
        French = 6,
        FrenchCanadian = 7,
        German = 8,
        Italian = 9,
        NorwegianDanish = 10,
        Spanish = 11,
        Swedish = 12,
        Swiss = 13,
        Turkish = 14,
        Portuguese = 15,
        Hebrew = 16,
        Greek = 17,
        Supplemental = 18,
        Technical = 19,
        Russian = 20
    };

    // Insert Replace Mode
    enum class IRMode
    {
        Insert = 0,
        Replace = 1
    };

    enum class ReportCommands
    {
        CursorPosition = 6,
        PrinterStatus = 15,
        UdkStatus = 25,
        KeyboardStatus = 26,
        LocatorStatus = 53,
        LocatorStatus2 = 55,
        LocatorType = 56,
        MacroSpace = 62,
        DataIntegrity = 75,
        MultiSessionConfig = 85
    };

    enum class DECCKM
    {
        Normal = 0,
        Application = 1
    };

    struct TerminalMode
    {
        ControlBits bits;
        int ansiConformanceLevel;
        CharacterSet charSet;
        CountryEncoding countryEnc;
        Keypad keypadMode;
        DECCKM decckm;
        bool altSendsEscape;
    };

    struct Input
    {
        /**
         *  a-z: 65-90
         *  0-9: 48-57
         *  0-9 keypad: 97-105
         *  / keypad: 111
         *  * keypad: 106
         *  - keypad: 109
         *  + keypad: 107
         *  , keypad: 110
         *  + keybd: 187
         *  # keybd: 191
         *  - keybd: 189
         *  . keybd: 190
         *  , keybd: 188
         *  context menu key: 93
         *  ^: 220
         *  F1-F12: 112-123
         *  tab: 9
         *  ess-zett: 219
         *  back-tick: 221
         *  backspace: 8
         *  enter: 13
         *  left-arrow: 37
         *  up-arrow: 38
         *  right-arrow: 39
         *  down-arrow: 40
         *  insert: 45
         *  home: 36
         *  pgup: 33
         *  pgdown: 34
         *  end: 35
         *  entf: 46
         *  numpad: 144
         *  shift lock: 20
         *  scroll: 145
         *  pause: 19
         *  oe: 192
         *  ae: 222
         *  ue: 186
         */
        int keyCode;
        bool ctrl;
        bool alt;
        bool shift;
        bool altGr;
    };

    std::string inputToSequence(Input const& input, TerminalMode mode);

    /**
     * @brief The TerminalController class Sends controls to terminal for mode and character set switching.
     */
    class TerminalController
    {
    public:
        /**
         * @brief changeCharacterSet Set character set / encoding
         * @param set The character set to choose
         * @param enc Additional encoding info, used if set is G0 to G3
         */
        std::string changeCharacterSet(CharacterSet set, CountryEncoding enc = CountryEncoding::_DEFAULT_CASE_IS_ERROR_IF_GSET_, bool returnAsString = false);

        /**
         * @brief switchToStandardCharacterSet Switches to G0
         */
        void switchToStandardCharacterSet();

        /**
         * @brief switchToStandardCharacterSet Switches to G1
         */
        void switchToAlternateCharacterSet();

        /**
         * @brief setControlBitness Set Terminal control sequence bitness.
         * @param bits
         */
        void setControlBitness(ControlBits bits);

        /**
         * @brief getTerminalId print terminal id to stdin.
         */
        void getTerminalId();

        /**
         * @brief setMode CSI Pm h Set Mode (SM)
         * @param modeParms
         */
        void setMode(std::vector <int> const& modeParams, bool set = true);

        /**
         * @brief setMode CSI Pm h Set Mode (SM)
         * @param modeParms
         */
        void setPrivateMode(std::vector <int> const& modeParams, bool set = true);

        /**
         * @brief setKeyboardActionMode Set action mode.
         */
        void lockKeyboard(bool lock);

        void setIrMode(IRMode mode);

        /**
         * @brief enableMouseMode Enable sending of mouse clicks
         */
        void enableMouseMode();

        /**
         * @brief enableFocusDectection Send me focus changes of the terminal.
         */
        void enableFocusDectection();

        /**
         * @brief printReport Request several terminal reports.
         * @param number
         */
        void printReport(std::vector <int> const& params);
        void printReport(std::vector <ReportCommands> const& params);

    private:
        ControlBits bits_ = ControlBits::S7C1T;
    };
}

#include <string>

// https://www.xfree86.org/current/ctlseqs.html

namespace TerminalControl
{
    enum class ControlBits
    {
        S7C1T,
        S8C1T
    };

    enum class Keypad
    {
        application, // DECPAM
        normal // DECPNM
    };

    enum class CharacterSet
    {
        ISO8859_1,
        UTF8,
        G0,
        G1,
        G2,
        G3
    };

    struct TerminalMode
    {
        ControlBits bits;
        int ansiConformanceLevel;
        CharacterSet charSet;
        char vt220charSet; // ESC+C
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
         *  ß: 219
         *  ´: 221
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
         *  ö: 192
         *  ä: 222
         *  ü: 186
         */
        int keyCode;
        bool ctrl;
        bool alt;
        bool shift;
    };

    std::string inputToSequence(Input const& input, TerminalMode mode);
}

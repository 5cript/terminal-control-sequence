#include "test_keys.hpp"

#include <iostream>
#include <iterator>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>

using namespace std::chrono_literals;

void key_print_loop()
{
    auto time = std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch()).count();

    auto nowMs = [&]()
    {
        return std::chrono::duration_cast <std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch()).count();;
    };
    auto refresh_time = [&]()
    {
        time = nowMs();
    };
    auto clear = []()
    {
        std::cout << "\x1b[2J\x1b[H" << std::flush;
    };

    std::atomic_bool lock{true};
    std::atomic_bool running{true};

    std::thread newliner{[&]()
    {
        for (;running.load();)
        {
            if (lock.load())
            {
                std::this_thread::sleep_for(100ms);
                continue;
            }

            if (nowMs() - time > 100)
            {
                refresh_time();
                lock.store(true);
            }
        }
    }};

    system("stty raw");

    //don't skip the whitespace while reading
    std::cin >> std::noskipws;

    // use stream iterators to copy the stream to a string
    std::istream_iterator<unsigned char> iter(std::cin);
    std::istream_iterator<unsigned char> end;

    for (; iter != end; ++iter)
    {
        if (lock.load())
        {
            clear();
        }

        auto i = *iter;
        if (i == '\x03')
        {
            std::cout << "ctrl + c pressed\r\n";
            break;
        }
        if (i <= 32 || i > 0x7E)
            std::cout << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)i;
        else
            std::cout << i;
        std::cout << ' ';

        refresh_time();
        lock.store(false);
    }

    running.store(false);
    newliner.join();
}

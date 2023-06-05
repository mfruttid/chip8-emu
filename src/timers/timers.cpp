#include "timers.h"
#include <chrono>
#include "../sound/sound.h"

void decreaseTimer(std::atomic<Chip8::Chip8::Register>& timer)
{
    static Sound sound { "/home/martina/cpp/chip8/sounds/test.wav" };
    sound.playSound();

    while (timer != 0)
    {
        const auto start = std::chrono::high_resolution_clock::now();

        --timer;

        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double, std::milli> sleep_time = std::chrono::milliseconds(1000/60) - (end - start);
        std::this_thread::sleep_for(sleep_time);
    }

    sound.pauseSound();
}


void decreaseDelayTimer(Chip8::Chip8& c)
{
    decreaseTimer(c.delayTimer);
}

void decreaseSoundTimer(Chip8::Chip8& c)
{
    decreaseTimer(c.soundTimer);
}

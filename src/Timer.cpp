#include <cstdint>
#include <chrono>

static int64_t MicrosecondsTimestamp()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    return microseconds.count();
}

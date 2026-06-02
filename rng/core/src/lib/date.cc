#include "./date.h"

std::string get_time()
{
    using namespace std::chrono;
    auto now = time_point_cast<milliseconds>(system_clock::now());
    return date::format("%T", now);
}

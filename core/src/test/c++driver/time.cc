#include "time.h"

std::string formatMilliseconds(long long ms) {
    using namespace std::chrono;

    // Convert milliseconds to hours, minutes, seconds, and milliseconds
    auto hrs = duration_cast<hours>(milliseconds(ms));
    ms -= duration_cast<milliseconds>(hrs).count();

    auto mins = duration_cast<minutes>(milliseconds(ms));
    ms -= duration_cast<milliseconds>(mins).count();

    auto secs = duration_cast<seconds>(milliseconds(ms));
    ms -= duration_cast<milliseconds>(secs).count();

    // Format the result as HH:MM:SS.mmm
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hrs.count() << ":"
        << std::setw(2) << std::setfill('0') << mins.count() << ":"
        << std::setw(2) << std::setfill('0') << secs.count() << "."
        << std::setw(3) << std::setfill('0') << ms;

    return oss.str();
}

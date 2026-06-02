#pragma once
#include <stdexcept>
#include <sstream>

template <typename... Args>
[[noreturn]] inline void throw_error(Args&&... args) {
    std::ostringstream oss;
    (oss << ... << args);
    throw std::logic_error(oss.str());
}

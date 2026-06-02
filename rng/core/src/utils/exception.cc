#include <iostream>
#include <exception>
#include <string>
#include "./exception.h"

RngException::RngException(const std::string& msg, const std::string& file, int line)
    : message(msg), _file(file), _line(line) 
{ }

const char* RngException::what() const noexcept {
    static std::string msg;
    msg = "RNG Exception: " + message + " (@" + _file + ":" + std::to_string(_line) + ")";
    return msg.c_str();
}

const char* RngException::file() {
    return _file.c_str();
}

int RngException::line() {
    return _line;
}

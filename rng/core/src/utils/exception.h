#pragma once

#include <iostream>
#include <exception>
#include <string>

class RngException : public std::exception {
    private:
        std::string message;
        std::string _file;
        int _line;

    public:
        RngException(const std::string& msg, const std::string& file, int line);

        const char* what() const noexcept override;
        const char* file();
        int line();
};

#define RNGEXC(msg) RngException(msg, __FILE__, __LINE__)

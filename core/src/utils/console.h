#pragma once
#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace console {
    template<typename T>
    std::string toString(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    template<>
    inline std::string toString<bool>(const bool& value) {
        return value ? "true" : "false";
    }

    // Main log function with {} placeholders
    template<typename... Args>
    void log(const std::string& format, const Args&... args) {
        std::vector<std::string> values = {toString(args)...};
        std::string result;
        size_t valueIndex = 0;

        for (size_t i = 0; i < format.length(); i++) {
            if (format[i] == '{' && i + 1 < format.length() && format[i + 1] == '}') {
                if (valueIndex < values.size()) {
                    result += values[valueIndex++];
                }
                i++; // Skip the '}'
            } else {
                result += format[i];
            }
        }

        std::cout << result << std::endl;
    }

    // Overload for no arguments
    inline void log(const std::string& message) {
        std::cout << message << std::endl;
    }

    // Version without newline (like process.stdout.write)
    template<typename... Args>
    void write(const std::string& format, const Args&... args) {
        std::vector<std::string> values = {toString(args)...};
        std::string result;
        size_t valueIndex = 0;

        for (size_t i = 0; i < format.length(); i++) {
            if (format[i] == '{' && i + 1 < format.length() && format[i + 1] == '}') {
                if (valueIndex < values.size()) {
                    result += values[valueIndex++];
                }
                i++; // Skip the '}'
            } else {
                result += format[i];
            }
        }

        std::cout << result;
    }

    inline void write(const std::string& message) {
        std::cout << message;
    }

    // Error output (to stderr)
    template<typename... Args>
    void error(const std::string& format, const Args&... args) {
        std::vector<std::string> values = {toString(args)...};
        std::string result;
        size_t valueIndex = 0;

        for (size_t i = 0; i < format.length(); i++) {
            if (format[i] == '{' && i + 1 < format.length() && format[i + 1] == '}') {
                if (valueIndex < values.size()) {
                    result += values[valueIndex++];
                }
                i++; // Skip the '}'
            } else {
                result += format[i];
            }
        }

        std::cerr << result << std::endl;
    }

    inline void error(const std::string& message) {
        std::cerr << message << std::endl;
    }

    // Warning output (to stderr with prefix)
    template<typename... Args>
    void warn(const std::string& format, const Args&... args) {
        std::cerr << "Warning: ";
        std::vector<std::string> values = {toString(args)...};
        std::string result;
        size_t valueIndex = 0;

        for (size_t i = 0; i < format.length(); i++) {
            if (format[i] == '{' && i + 1 < format.length() && format[i + 1] == '}') {
                if (valueIndex < values.size()) {
                    result += values[valueIndex++];
                }
                i++; // Skip the '}'
            } else {
                result += format[i];
            }
        }

        std::cerr << result << std::endl;
    }

    inline void warn(const std::string& message) {
        std::cerr << "Warning: " << message << std::endl;
    }

    // Clear console (platform-specific)
    inline void clear() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
}

#endif // CONSOLE_H

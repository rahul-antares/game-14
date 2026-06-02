#include "locale.h"

std::string formatDouble(double number) {
    try {
        // Set the locale
        std::locale loc("en_US.UTF-8");
        std::locale::global(loc); // Set global locale (optional)

        // Create a stringstream with the desired locale
        std::ostringstream oss;
        oss.imbue(loc); // Set the locale for the stream

        // Format the number
        oss << std::fixed << std::setprecision(2) << number;
        return oss.str();
    } catch (const std::runtime_error& e) {
        std::cerr << "Error setting locale: " << e.what() << std::endl;
        return std::to_string(number); // Fallback if locale is unavailable
    }
}

std::string formatInt(unsigned long long number) {
    try {
        // Set the locale
        std::locale loc("en_US.UTF-8");
        std::locale::global(loc); // Set global locale (optional)

        // Create a stringstream with the desired locale
        std::ostringstream oss;
        oss.imbue(loc); // Set the locale for the stream

        // Format the number
        oss << std::fixed << std::setprecision(0) << number;
        return oss.str();
    } catch (const std::runtime_error& e) {
        std::cerr << "Error setting locale: " << e.what() << std::endl;
        return std::to_string(number); // Fallback if locale is unavailable
    }
}

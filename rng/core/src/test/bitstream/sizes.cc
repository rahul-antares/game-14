#include <cstdint>
#include <iostream>
#include <iomanip>
#include <string>
#include <cctype>
#include <unordered_map>

uint64_t parse_size(const std::string& input) {
    static const std::unordered_map<char, uint64_t> suffixes = {
        {'K', 1024ULL},          // 1K = 1024 bytes
        {'M', 1024ULL * 1024},   // 1M = 1024 * 1024 bytes
        {'G', 1024ULL * 1024 * 1024}, // 1G = 1024^3 bytes
        {'T', 1024ULL * 1024 * 1024 * 1024}  // 1T = 1024^4 bytes
    };

    // Extract numeric part
    size_t i = 0;
    while (i < input.size() && std::isdigit(input[i])) {
        ++i;
    }

    // Convert number to integer
    uint64_t value = std::stoull(input.substr(0, i));

    // Check if there's a suffix (e.g., "K", "M", etc.)
    if (i < input.size()) {
        char suffix = std::toupper(input[i]); // Normalize case
        auto it = suffixes.find(suffix);

        if (it != suffixes.end()) {
            value *= it->second;  // Apply multiplier
        } else {
            throw std::invalid_argument("Invalid size suffix: " + input);
        }
    }

    return value;
}

std::string humanReadableSize(size_t bytes) {
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB"};
    size_t suffixIndex = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024 && suffixIndex < (sizeof(suffixes) / sizeof(suffixes[0])) - 1) {
        size /= 1024.0;
        ++suffixIndex;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << suffixes[suffixIndex];
    return oss.str();
}

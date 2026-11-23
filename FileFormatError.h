#pragma once

#include <stdexcept>
#include <string>

class FileFormatError : public std::runtime_error {
public:
    explicit FileFormatError(const std::string& message) : std::runtime_error(message) {}
};

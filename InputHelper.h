#pragma once

#include <stdexcept>
#include <string>

class InputClosedError : public std::runtime_error {
public:
    InputClosedError() : std::runtime_error("Input closed. Exiting without saving.") {}
};

namespace InputHelper {
std::string readLine(const std::string& prompt);
std::string readNonEmpty(const std::string& prompt);
std::string readOptional(const std::string& prompt, const std::string& current);
int readInt(const std::string& prompt, int min, int max);
int readNonNegativeInt(const std::string& prompt);
double readPositiveDouble(const std::string& prompt);
int readOptionalNonNegativeInt(const std::string& prompt, int current);
double readOptionalPositiveDouble(const std::string& prompt, double current);
bool readYesNo(const std::string& prompt);
}

#include "InputHelper.h"
#include "StringUtils.h"

#include <iostream>
#include <optional>

namespace {
std::string editPrompt(const std::string& label, const std::string& current) {
    return label + " [" + current + "] (Enter to keep): ";
}

std::string readText(const std::string& prompt, const std::string& current) {
    while (true) {
        const auto value = InputHelper::readLine(prompt);
        if (value.empty() && !current.empty()) {
            return current;
        }
        if (value.empty()) {
            std::cout << "Value must not be empty.\n";
        } else if (StringUtils::hasComma(value)) {
            std::cout << "Value must not contain ','.\n";
        } else {
            return value;
        }
    }
}

int readAge(const std::string& prompt, std::optional<int> current) {
    while (true) {
        const auto text = InputHelper::readLine(prompt);
        if (text.empty() && current.has_value()) {
            return *current;
        }
        int value = 0;
        if (StringUtils::tryParseInt(text, value) && value >= 0) {
            return value;
        }
        std::cout << "Age must be a whole number >= 0.\n";
    }
}

double readFee(const std::string& prompt, std::optional<double> current) {
    while (true) {
        const auto text = InputHelper::readLine(prompt);
        if (text.empty() && current.has_value()) {
            return *current;
        }
        double value = 0;
        if (StringUtils::tryParseDouble(text, value) && value > 0) {
            return value;
        }
        std::cout << "Service fee must be a number greater than 0.\n";
    }
}
}

std::string InputHelper::readLine(const std::string& prompt) {
    std::cout << prompt << std::flush;
    std::string line;
    if (!std::getline(std::cin, line)) {
        throw InputClosedError();
    }
    return StringUtils::trim(line);
}

std::string InputHelper::readNonEmpty(const std::string& prompt) {
    return readText(prompt, "");
}

std::string InputHelper::readOptional(const std::string& prompt, const std::string& current) {
    return readText(editPrompt(prompt, current), current);
}

int InputHelper::readInt(const std::string& prompt, int min, int max) {
    while (true) {
        int value = 0;
        if (StringUtils::tryParseInt(readLine(prompt), value) && value >= min && value <= max) {
            return value;
        }
        std::cout << "Invalid choice.\n";
    }
}

int InputHelper::readNonNegativeInt(const std::string& prompt) {
    return readAge(prompt, std::nullopt);
}

double InputHelper::readPositiveDouble(const std::string& prompt) {
    return readFee(prompt, std::nullopt);
}

int InputHelper::readOptionalNonNegativeInt(const std::string& prompt, int current) {
    return readAge(editPrompt(prompt, std::to_string(current)), current);
}

double InputHelper::readOptionalPositiveDouble(const std::string& prompt, double current) {
    return readFee(editPrompt(prompt, StringUtils::formatDouble(current)), current);
}

bool InputHelper::readYesNo(const std::string& prompt) {
    while (true) {
        const auto answer = StringUtils::toLower(readLine(prompt));
        if (answer == "y" || answer == "n") {
            return answer == "y";
        }
        std::cout << "Please enter y or n.\n";
    }
}

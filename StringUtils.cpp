#include "StringUtils.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <codecvt>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace {
// Keep Vietnamese case matching independent of the computer's installed locales.
constexpr std::u32string_view upperLetters =
    U"ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁẢÃẠĂẰẮẲẴẶÂẦẤẨẪẬĐÈÉẺẼẸÊỀẾỂỄỆ"
    U"ÌÍỈĨỊÒÓỎÕỌÔỒỐỔỖỘƠỜỚỞỠỢÙÚỦŨỤƯỪỨỬỮỰỲÝỶỸỴ";
constexpr std::u32string_view lowerLetters =
    U"abcdefghijklmnopqrstuvwxyzàáảãạăằắẳẵặâầấẩẫậđèéẻẽẹêềếểễệ"
    U"ìíỉĩịòóỏõọôồốổỗộơờớởỡợùúủũụưừứửữựỳýỷỹỵ";

std::string changeCase(std::string value, bool uppercase) {
    const auto from = uppercase ? lowerLetters : upperLetters;
    const auto to = uppercase ? upperLetters : lowerLetters;
    try {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf8;
        auto letters = utf8.from_bytes(value);
        for (auto& letter : letters) {
            const auto pos = from.find(letter);
            if (pos != std::u32string_view::npos) {
                letter = to[pos];
            }
        }
        return utf8.to_bytes(letters);
    } catch (const std::range_error&) {
        // Older text files may contain local-code-page bytes. Preserve those bytes.
        for (auto& letter : value) {
            const auto byte = static_cast<unsigned char>(letter);
            letter = static_cast<char>(uppercase ? std::toupper(byte) : std::tolower(byte));
        }
        return value;
    }
}
}

std::string StringUtils::trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    return value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
}

std::string StringUtils::toLower(std::string value) { return changeCase(std::move(value), false); }
std::string StringUtils::toUpper(std::string value) { return changeCase(std::move(value), true); }

std::vector<std::string> StringUtils::split(const std::string& line, char separator) {
    std::vector<std::string> fields;
    std::size_t start = 0;
    while (true) {
        const auto end = line.find(separator, start);
        fields.push_back(trim(line.substr(start, end - start)));
        if (end == std::string::npos) {
            return fields;
        }
        start = end + 1;
    }
}

bool StringUtils::containsIgnoreCase(const std::string& text, const std::string& key) {
    return toLower(text).find(toLower(key)) != std::string::npos;
}

bool StringUtils::hasComma(const std::string& value) {
    return value.find(',') != std::string::npos;
}

bool StringUtils::tryParseInt(const std::string& text, int& result) {
    try {
        std::size_t end = 0;
        const int parsed = std::stoi(text, &end);
        if (end != text.size()) {
            return false;
        }
        result = parsed;
        return true;
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

bool StringUtils::tryParseDouble(const std::string& text, double& result) {
    try {
        std::size_t end = 0;
        const double parsed = std::stod(text, &end);
        if (end != text.size() || !std::isfinite(parsed)) {
            return false;
        }
        result = parsed;
        return true;
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

std::string StringUtils::formatDouble(double value) {
    std::ostringstream out;
    out << std::setprecision(std::numeric_limits<double>::digits10) << value;
    double parsed = 0;
    if (!tryParseDouble(out.str(), parsed) || parsed != value) {
        out.str("");
        out << std::setprecision(std::numeric_limits<double>::max_digits10) << value;
    }
    return out.str();
}

int StringUtils::columnWidth(const std::string& value, int characters) {
    // setw counts bytes. Continuation bytes occupy no extra column in UTF-8 Vietnamese.
    const auto extraBytes = std::count_if(value.begin(), value.end(), [](unsigned char byte) {
        return (byte & 0xc0) == 0x80;
    });
    return characters + static_cast<int>(extraBytes);
}

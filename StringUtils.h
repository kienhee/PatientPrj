#pragma once

#include <string>
#include <vector>

namespace StringUtils {
std::string trim(const std::string& value);
std::string toLower(std::string value);
std::string toUpper(std::string value);
std::vector<std::string> split(const std::string& line, char separator);
bool containsIgnoreCase(const std::string& text, const std::string& key);
bool hasComma(const std::string& value);
bool tryParseInt(const std::string& text, int& result);
bool tryParseDouble(const std::string& text, double& result);
std::string formatDouble(double value);
int columnWidth(const std::string& value, int characters);
}

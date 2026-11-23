#pragma once

#include "FileFormatError.h"
#include "StringUtils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <utility>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace RecordFile {
inline void replaceFile(const std::filesystem::path& source, const std::filesystem::path& target,
                        std::error_code& error) {
#ifdef _WIN32
    // TDM-GCC 9's filesystem::rename cannot replace an existing Windows file.
    if (!MoveFileExW(source.c_str(), target.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        error = std::error_code(static_cast<int>(GetLastError()), std::system_category());
    }
#else
    std::filesystem::rename(source, target, error);
#endif
}

inline std::vector<std::string> fields(const std::string& line, std::size_t expected) {
    auto values = StringUtils::split(line, ',');
    if (values.size() != expected) {
        throw FileFormatError("expected " + std::to_string(expected) + " fields, got " +
                              std::to_string(values.size()));
    }
    if (std::find(values.begin(), values.end(), "") != values.end()) {
        throw FileFormatError("empty field");
    }
    return values;
}

template <typename T, typename Parser>
bool load(const std::string& filename, const std::string& kind, std::vector<T>& rows, Parser parse) {
    std::ifstream input(filename);
    if (!input) {
        std::cout << "Cannot open file: " << filename << '\n';
        return false;
    }
    std::vector<T> loaded;
    std::unordered_set<std::string> ids;
    std::size_t lineNumber = 0;
    std::size_t skipped = 0;
    std::string line;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (lineNumber == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3); // UTF-8 byte order mark, common in Windows text files.
        }
        if (StringUtils::trim(line).empty()) {
            continue;
        }
        try {
            T record = parse(line);
            const auto id = record.getID();
            if (!ids.insert(id).second) {
                throw FileFormatError("duplicate ID " + id);
            }
            loaded.push_back(std::move(record));
        } catch (const FileFormatError& error) {
            ++skipped;
            std::cout << filename << " line " << lineNumber << ": " << error.what() << '\n';
        }
    }
    if (input.bad() || !input.eof()) {
        std::cout << "Cannot read file: " << filename << '\n';
        return false;
    }
    rows.swap(loaded);
    std::cout << "Loaded " << rows.size() << ' ' << kind << " (" << skipped << " line(s) skipped).\n";
    return true;
}

template <typename T>
bool save(const std::string& filename, const std::string& kind, const std::vector<T>& rows) {
    namespace fs = std::filesystem;
    // Reserve a private staging directory beside the destination. Only a complete
    // file replaces the original, even when the disk fills up during the write.
    fs::path staging;
    std::error_code error;
    for (unsigned int attempt = 0; ; ++attempt) {
        staging = filename + ".saving-" + std::to_string(attempt);
        if (fs::create_directory(staging, error)) {
            break;
        }
        if (error && error != std::errc::file_exists) {
            std::cout << "Cannot open file: " << filename << " (" << error.message() << ")\n";
            return false;
        }
    }

    const fs::path temporary = staging / "records.txt";
    std::ofstream output(temporary);
    for (const auto& record : rows) {
        output << record.toString() << '\n';
    }
    output.close(); // A write error may first be reported when closing.
    if (output) {
        replaceFile(temporary, filename, error);
    }
    const bool saved = output && !error;
    std::error_code cleanupError;
    fs::remove(temporary, cleanupError);
    fs::remove(staging, cleanupError);
    if (!saved) {
        std::cout << "Cannot write file: " << filename;
        if (error) {
            std::cout << " (" << error.message() << ')';
        }
        std::cout << '\n';
        return false;
    }
    std::cout << "Saved " << rows.size() << ' ' << kind << " to " << filename << ".\n";
    return true;
}
}

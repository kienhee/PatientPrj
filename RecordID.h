#pragma once

#include "InputHelper.h"
#include "StringUtils.h"

#include <iostream>

namespace RecordID {
template <typename T>
int find(const std::vector<T>& rows, const std::string& id) {
    const auto key = StringUtils::toUpper(StringUtils::trim(id));
    for (std::size_t i = 0; i < rows.size(); ++i) {
        if (rows[i].getID() == key) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

template <typename List>
std::string readUnique(const List& list, const std::string& label, const std::string& current = "") {
    while (true) {
        const std::string input = current.empty() ? InputHelper::readNonEmpty(label + ": ")
                                                  : InputHelper::readOptional(label, current);
        const auto id = StringUtils::toUpper(input);
        if (id == current || list.searchID(id) == -1) {
            return id;
        }
        std::cout << label << " already exists.\n";
    }
}
}

#pragma once

#include "InputHelper.h"
#include "Record.h"

#include <vector>

class Menu {
public:
    template <typename T>
    static int int_getChoice(const std::vector<T>& options, const std::string& title,
                             bool withExit = false) {
        if (options.empty() && !withExit) {
            throw std::invalid_argument("Cannot choose from an empty menu.");
        }
        std::cout << "\n===== " << title << " =====\n";
        for (std::size_t i = 0; i < options.size(); ++i) {
            std::cout << i + 1 << ". " << label(options[i]) << '\n';
        }
        if (withExit) {
            std::cout << "0. Exit\n";
        }
        return InputHelper::readInt("Your choice: ", withExit ? 0 : 1, static_cast<int>(options.size()));
    }

    template <typename T>
    static const T& ref_getChoice(const std::vector<T>& options, const std::string& title) {
        return options.at(int_getChoice(options, title) - 1);
    }

private:
    static std::string label(const std::string& text) { return text; }
    static std::string label(const Record& record) { return record.screenString(); }
};

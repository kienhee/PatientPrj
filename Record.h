#pragma once

#include <iostream>
#include <string>

class Record {
public:
    virtual ~Record() = default;
    virtual std::string getID() const = 0;
    virtual std::string toString() const = 0;
    virtual std::string screenString() const = 0;
};

inline void printRecord(const Record& record) {
    std::cout << record.screenString() << '\n';
}

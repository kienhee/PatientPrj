#pragma once

#include "Department.h"

#include <vector>

class DepartmentList {
private:
    std::vector<Department> departments;
    bool dirty = false;
    Department parseLine(const std::string& line) const;

public:
    DepartmentList() = default;
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename);
    int searchID(const std::string& id) const;
    Department getUserChoice() const;
    void addDepartment();
    int updateDepartment(std::string& oldID);
    void searchDepartment() const;
    void listDepartments() const;
    const Department& at(int pos) const;
    int size() const;
    bool isEmpty() const;
    bool isDirty() const;
};

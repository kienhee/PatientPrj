#include "DepartmentList.h"
#include "InputHelper.h"
#include "Menu.h"
#include "RecordFile.h"
#include "RecordID.h"

Department DepartmentList::parseLine(const std::string& line) const {
    const auto values = RecordFile::fields(line, 4);
    double fee = 0;
    if (!StringUtils::tryParseDouble(values[3], fee)) {
        throw FileFormatError("invalid serviceFee '" + values[3] + "'");
    }
    if (fee <= 0) {
        throw FileFormatError("serviceFee must be > 0");
    }
    return Department(values[0], values[1], values[2], fee);
}

bool DepartmentList::loadFromFile(const std::string& filename) {
    if (!RecordFile::load(filename, "departments", departments,
                          [this](const std::string& line) { return parseLine(line); })) {
        return false;
    }
    dirty = false;
    return true;
}

bool DepartmentList::saveToFile(const std::string& filename) {
    if (!RecordFile::save(filename, "departments", departments)) {
        return false;
    }
    dirty = false;
    return true;
}

int DepartmentList::searchID(const std::string& id) const {
    return RecordID::find(departments, id);
}

Department DepartmentList::getUserChoice() const {
    return Menu::ref_getChoice(departments, "SELECT DEPARTMENT");
}

void DepartmentList::addDepartment() {
    const auto id = RecordID::readUnique(*this, "Department ID");
    const auto name = InputHelper::readNonEmpty("Department name: ");
    const auto floorName = InputHelper::readNonEmpty("Floor: ");
    const auto fee = InputHelper::readPositiveDouble("Service fee: ");
    departments.emplace_back(id, name, floorName, fee);
    dirty = true;
    std::cout << "Department added.\n";
}

void DepartmentList::searchDepartment() const {
    const int pos = searchID(InputHelper::readNonEmpty("Department ID: "));
    if (pos == -1) {
        std::cout << "Department not found.\n";
        return;
    }
    std::cout << Department::screenHeader() << '\n';
    printRecord(at(pos));
}

int DepartmentList::updateDepartment(std::string& oldID) {
    const int pos = searchID(InputHelper::readNonEmpty("Department ID to update: "));
    if (pos == -1) {
        std::cout << "Department not found.\n";
        return -1;
    }
    const Department& current = at(pos);
    oldID = current.getID();
    const auto id = RecordID::readUnique(*this, "Department ID", oldID);
    const auto name = InputHelper::readOptional("Department name", current.getDepartmentName());
    const auto floorName = InputHelper::readOptional("Floor", current.getFloor());
    const auto fee = InputHelper::readOptionalPositiveDouble("Service fee", current.getServiceFee());
    departments[pos] = Department(id, name, floorName, fee);
    dirty = true;
    std::cout << "Department updated.\n" << Department::screenHeader() << '\n';
    printRecord(at(pos));
    return pos;
}

void DepartmentList::listDepartments() const {
    if (isEmpty()) {
        std::cout << "No department.\n";
        return;
    }
    std::cout << Department::screenHeader() << '\n';
    for (const auto& dept : departments) {
        printRecord(dept);
    }
    std::cout << "Total: " << size() << '\n';
}

const Department& DepartmentList::at(int pos) const { return departments.at(pos); }
int DepartmentList::size() const { return static_cast<int>(departments.size()); }
bool DepartmentList::isEmpty() const { return departments.empty(); }
bool DepartmentList::isDirty() const { return dirty; }

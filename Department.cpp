#include "Department.h"
#include "StringUtils.h"

#include <iomanip>
#include <sstream>
#include <utility>

namespace {
std::string formatRow(const std::string& id, const std::string& name,
                      const std::string& floorName, const std::string& fee) {
    std::ostringstream row;
    row << std::left << std::setw(StringUtils::columnWidth(id, 6)) << id << ' '
        << std::setw(StringUtils::columnWidth(name, 21)) << name << ' '
        << std::setw(StringUtils::columnWidth(floorName, 6)) << floorName << ' '
        << std::right << std::setw(12) << fee;
    return row.str();
}
}

Department::Department() = default;

Department::Department(std::string id, std::string name, std::string floorName, double fee)
    : departmentID(StringUtils::toUpper(std::move(id))), departmentName(std::move(name)),
      floor(std::move(floorName)), serviceFee(fee) {}

const std::string& Department::getDepartmentID() const { return departmentID; }
const std::string& Department::getDepartmentName() const { return departmentName; }
const std::string& Department::getFloor() const { return floor; }
double Department::getServiceFee() const { return serviceFee; }

void Department::setDepartmentID(const std::string& value) { departmentID = StringUtils::toUpper(value); }
void Department::setDepartmentName(const std::string& value) { departmentName = value; }
void Department::setFloor(const std::string& value) { floor = value; }
void Department::setServiceFee(double value) { serviceFee = value; }

std::string Department::getID() const { return departmentID; }

std::string Department::toString() const {
    return departmentID + ',' + departmentName + ',' + floor + ',' + StringUtils::formatDouble(serviceFee);
}

std::string Department::screenHeader() {
    return formatRow("ID", "Name", "Floor", "Fee") + '\n' + std::string(48, '-');
}

std::string Department::screenString() const {
    return formatRow(departmentID, departmentName, floor, StringUtils::formatDouble(serviceFee));
}

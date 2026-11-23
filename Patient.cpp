#include "Patient.h"
#include "StringUtils.h"

#include <iomanip>
#include <sstream>
#include <utility>

namespace {
std::string formatRow(const std::string& id, const std::string& deptID,
                      const std::string& deptName, const std::string& name,
                      const std::string& diagnosis, const std::string& age) {
    std::ostringstream row;
    row << std::left << std::setw(StringUtils::columnWidth(id, 5)) << id << ' '
        << std::setw(StringUtils::columnWidth(deptID, 7)) << deptID << ' '
        << std::setw(StringUtils::columnWidth(deptName, 17)) << deptName << ' '
        << std::setw(StringUtils::columnWidth(name, 19)) << name << ' '
        << std::setw(StringUtils::columnWidth(diagnosis, 17)) << diagnosis << ' '
        << std::right << std::setw(4) << age;
    return row.str();
}
}

Patient::Patient() = default;

Patient::Patient(std::string id, Department dept, std::string name, std::string diag, int ageValue)
    : patientID(StringUtils::toUpper(std::move(id))), department(std::move(dept)),
      fullName(std::move(name)), diagnosis(std::move(diag)), age(ageValue) {}

const std::string& Patient::getPatientID() const { return patientID; }
const Department& Patient::getDepartment() const { return department; }
const std::string& Patient::getFullName() const { return fullName; }
const std::string& Patient::getDiagnosis() const { return diagnosis; }
int Patient::getAge() const { return age; }

void Patient::setPatientID(const std::string& value) { patientID = StringUtils::toUpper(value); }
void Patient::setDepartment(const Department& value) { department = value; }
void Patient::setFullName(const std::string& value) { fullName = value; }
void Patient::setDiagnosis(const std::string& value) { diagnosis = value; }
void Patient::setAge(int value) { age = value; }

std::string Patient::getID() const { return patientID; }

std::string Patient::toString() const {
    return patientID + ',' + department.getID() + ',' + fullName + ',' + diagnosis + ',' + std::to_string(age);
}

std::string Patient::screenHeader() {
    return formatRow("ID", "Dept ID", "Dept name", "Full name", "Diagnosis", "Age") +
           '\n' + std::string(74, '-');
}

std::string Patient::screenString() const {
    return formatRow(patientID, department.getID(), department.getDepartmentName(),
                     fullName, diagnosis, std::to_string(age));
}

bool Patient::operator<(const Patient& other) const {
    const auto name = StringUtils::toLower(department.getDepartmentName());
    const auto otherName = StringUtils::toLower(other.department.getDepartmentName());
    if (name == otherName) {
        return patientID < other.patientID;
    }
    return name < otherName;
}

#include "PatientList.h"
#include "InputHelper.h"
#include "RecordFile.h"
#include "RecordID.h"

#include <algorithm>

PatientList::PatientList(const DepartmentList& departments) : departmentList(departments) {}

Patient PatientList::parseLine(const std::string& line) const {
    const auto values = RecordFile::fields(line, 5);
    const int deptPos = departmentList.searchID(values[1]);
    if (deptPos == -1) {
        throw FileFormatError("department " + StringUtils::toUpper(values[1]) + " does not exist");
    }
    int age = 0;
    if (!StringUtils::tryParseInt(values[4], age)) {
        throw FileFormatError("invalid age '" + values[4] + "'");
    }
    if (age < 0) {
        throw FileFormatError("age must not be negative");
    }
    return Patient(values[0], departmentList.at(deptPos), values[2], values[3], age);
}

bool PatientList::loadFromFile(const std::string& filename) {
    if (!RecordFile::load(filename, "patients", patients,
                          [this](const std::string& line) { return parseLine(line); })) {
        return false;
    }
    dirty = false;
    return true;
}

bool PatientList::saveToFile(const std::string& filename) {
    if (!RecordFile::save(filename, "patients", patients)) {
        return false;
    }
    dirty = false;
    return true;
}

int PatientList::searchID(const std::string& id) const {
    return RecordID::find(patients, id);
}

void PatientList::addPatient() {
    if (departmentList.isEmpty()) {
        std::cout << "No department available. Please add a department first.\n";
        return;
    }
    const auto id = RecordID::readUnique(*this, "Patient ID");
    const auto dept = departmentList.getUserChoice();
    const auto name = InputHelper::readNonEmpty("Full name: ");
    const auto diagnosis = InputHelper::readNonEmpty("Diagnosis: ");
    const auto age = InputHelper::readNonNegativeInt("Age: ");
    patients.emplace_back(id, dept, name, diagnosis, age);
    dirty = true;
    std::cout << "Patient added.\n";
}

bool PatientList::removePatient() {
    const int pos = searchID(InputHelper::readNonEmpty("Patient ID to remove: "));
    if (pos == -1) {
        std::cout << "Patient not found.\n";
        return false;
    }
    std::cout << Patient::screenHeader() << '\n';
    printRecord(patients[pos]);
    if (!InputHelper::readYesNo("Remove this patient? (y/n): ")) {
        std::cout << "Cancelled.\n";
        return false;
    }
    patients.erase(patients.begin() + pos);
    dirty = true;
    std::cout << "Patient removed.\n";
    return true;
}

bool PatientList::updatePatient() {
    const int pos = searchID(InputHelper::readNonEmpty("Patient ID to update: "));
    if (pos == -1) {
        std::cout << "Patient not found.\n";
        return false;
    }
    const Patient& current = patients[pos];
    const auto id = RecordID::readUnique(*this, "Patient ID", current.getID());
    Department dept = current.getDepartment();
    std::cout << "Current department: " << dept.getID() << " - " << dept.getDepartmentName() << '\n';
    if (InputHelper::readYesNo("Change department? (y/n): ")) {
        dept = departmentList.getUserChoice();
    }
    const auto name = InputHelper::readOptional("Full name", current.getFullName());
    const auto diagnosis = InputHelper::readOptional("Diagnosis", current.getDiagnosis());
    const auto age = InputHelper::readOptionalNonNegativeInt("Age", current.getAge());
    patients[pos] = Patient(id, dept, name, diagnosis, age);
    dirty = true;
    std::cout << "Patient updated.\n" << Patient::screenHeader() << '\n';
    printRecord(patients[pos]);
    return true;
}

void PatientList::printTable(const std::vector<Patient>& rows) {
    std::cout << Patient::screenHeader() << '\n';
    for (const auto& patient : rows) {
        printRecord(patient);
    }
    std::cout << "Total: " << rows.size() << '\n';
}

void PatientList::listPatients() const {
    if (patients.empty()) {
        std::cout << "No patient.\n";
        return;
    }
    auto sorted = patients;
    std::sort(sorted.begin(), sorted.end());
    printTable(sorted);
}

void PatientList::printBasedDepartmentName() const {
    const auto key = InputHelper::readNonEmpty("Part of department name: ");
    std::vector<Patient> matches;
    for (const auto& patient : patients) {
        if (StringUtils::containsIgnoreCase(patient.getDepartment().getDepartmentName(), key)) {
            matches.push_back(patient);
        }
    }
    if (matches.empty()) {
        std::cout << "No patient found.\n";
        return;
    }
    std::sort(matches.begin(), matches.end());
    printTable(matches);
}

int PatientList::syncDepartment(const std::string& oldID, const Department& dept) {
    const auto key = StringUtils::toUpper(StringUtils::trim(oldID));
    int updated = 0;
    for (auto& patient : patients) {
        if (patient.getDepartment().getID() == key) {
            patient.setDepartment(dept);
            ++updated;
        }
    }
    // The department ID is the only department field stored in patients.txt.
    if (updated > 0 && key != dept.getID()) {
        dirty = true;
    }
    return updated;
}

bool PatientList::isDirty() const { return dirty; }

#pragma once

#include "DepartmentList.h"
#include "Patient.h"

class PatientList {
private:
    const DepartmentList& departmentList;
    std::vector<Patient> patients;
    bool dirty = false;
    Patient parseLine(const std::string& line) const;
    static void printTable(const std::vector<Patient>& rows);

public:
    explicit PatientList(const DepartmentList& departments);
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename);
    int searchID(const std::string& id) const;
    void addPatient();
    bool removePatient();
    bool updatePatient();
    void listPatients() const;
    void printBasedDepartmentName() const;
    int syncDepartment(const std::string& oldID, const Department& dept);
    bool isDirty() const;
};

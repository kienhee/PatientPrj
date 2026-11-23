#pragma once

#include "Department.h"

class Patient : public Record {
private:
    std::string patientID;
    Department department;
    std::string fullName;
    std::string diagnosis;
    int age = 0;

public:
    Patient();
    Patient(std::string id, Department dept, std::string name, std::string diag, int ageValue);

    const std::string& getPatientID() const;
    const Department& getDepartment() const;
    const std::string& getFullName() const;
    const std::string& getDiagnosis() const;
    int getAge() const;

    void setPatientID(const std::string& value);
    void setDepartment(const Department& value);
    void setFullName(const std::string& value);
    void setDiagnosis(const std::string& value);
    void setAge(int value);

    std::string getID() const override;
    std::string toString() const override;
    std::string screenString() const override;
    static std::string screenHeader();
    bool operator<(const Patient& other) const;
};

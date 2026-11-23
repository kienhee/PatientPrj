#pragma once

#include "Record.h"

class Department : public Record {
private:
    std::string departmentID;
    std::string departmentName;
    std::string floor;
    double serviceFee = 0;

public:
    Department();
    Department(std::string id, std::string name, std::string floorName, double fee);

    const std::string& getDepartmentID() const;
    const std::string& getDepartmentName() const;
    const std::string& getFloor() const;
    double getServiceFee() const;

    void setDepartmentID(const std::string& value);
    void setDepartmentName(const std::string& value);
    void setFloor(const std::string& value);
    void setServiceFee(double value);

    std::string getID() const override;
    std::string toString() const override;
    std::string screenString() const override;
    static std::string screenHeader();
};

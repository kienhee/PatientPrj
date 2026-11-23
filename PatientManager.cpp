#include "DepartmentList.h"
#include "Menu.h"
#include "PatientList.h"

namespace {
const std::string departmentFile = "departments.txt";
const std::string patientFile = "patients.txt";

bool confirmExit(const DepartmentList& departments, const PatientList& patients) {
    if (!departments.isDirty() && !patients.isDirty()) {
        return true;
    }
    if (departments.isDirty()) {
        std::cout << departmentFile << " has unsaved changes.\n";
    }
    if (patients.isDirty()) {
        std::cout << patientFile << " has unsaved changes.\n";
    }
    return InputHelper::readYesNo("Exit without saving? (y/n): ");
}

void updateDepartment(DepartmentList& departments, PatientList& patients) {
    std::string oldID;
    const int pos = departments.updateDepartment(oldID);
    if (pos == -1) {
        return;
    }
    const auto& updated = departments.at(pos);
    const int count = patients.syncDepartment(oldID, updated);
    if (count > 0 && oldID != updated.getID()) {
        std::cout << count << " patient(s) updated. Save both departments.txt (5) and patients.txt (11).\n";
    }
}
}

int main() {
    DepartmentList departments;
    departments.loadFromFile(departmentFile);
    PatientList patients(departments);
    patients.loadFromFile(patientFile);

    const std::vector<std::string> options = {
        "List all Departments",
        "Add a new Department",
        "Search Department by ID",
        "Update Department",
        "Save Departments to departments.txt",
        "List all Patients sorted by departmentName",
        "Search Patients by partial departmentName",
        "Add a new Patient",
        "Remove Patient by ID",
        "Update Patient by ID",
        "Save Patients to patients.txt"
    };

    try {
        while (true) {
            const int choice = Menu::int_getChoice(options, "PATIENT MANAGEMENT SYSTEM", true);
            switch (choice) {
                case 1: departments.listDepartments(); break;
                case 2: departments.addDepartment(); break;
                case 3: departments.searchDepartment(); break;
                case 4: updateDepartment(departments, patients); break;
                case 5: departments.saveToFile(departmentFile); break;
                case 6: patients.listPatients(); break;
                case 7: patients.printBasedDepartmentName(); break;
                case 8: patients.addPatient(); break;
                case 9: patients.removePatient(); break;
                case 10: patients.updatePatient(); break;
                case 11: patients.saveToFile(patientFile); break;
                case 0:
                    if (confirmExit(departments, patients)) {
                        return 0;
                    }
                    break;
            }
        }
    } catch (const InputClosedError& error) {
        std::cout << '\n' << error.what() << '\n';
    }
    return 0;
}

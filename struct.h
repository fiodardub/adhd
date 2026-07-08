#pragma once
#include <QString>

struct Employee {
    QString name;
    QString position;
    qint16 salary;
    QString iconPath;

    bool operator<(const Employee& other) const {
        return this->salary < other.salary;
    }
};

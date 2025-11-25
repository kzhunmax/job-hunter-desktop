#include "Job.h"

Job::Job(const long id, const QString &title, const QString &companyName, const double salary) {
    this->id = id;
    this->title = title;
    this->companyName = companyName;
    this->salary = salary;
}

long Job::getId() const { return id;}
QString Job::getCompanyName() const { return companyName;}
QString Job::getTitle() const { return title;}
QString Job::getSalaryString() const {
    return "$" + QString::number(salary, 'f', 2);
}

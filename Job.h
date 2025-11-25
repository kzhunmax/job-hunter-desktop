#ifndef JOB_H
#define JOB_H

#include <QString> // provide many function to work with strings

// encapsulation for field, getters methods to access private fields
class Job {
private:
    long id;
    QString title;
    QString companyName;
    double salary;
public:
    Job(long id, const QString &title, const QString &companyName, double salary);

    long getId() const;
    QString getTitle() const;
    QString getCompanyName() const;
    QString getSalaryString()const;
};

#endif //JOB_H
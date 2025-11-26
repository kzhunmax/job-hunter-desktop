#ifndef JOBCARD_H
#define JOBCARD_H
#include <QWidget>
#include "Job.h"

class JobCard : public QWidget {
    Q_OBJECT;
public:
    explicit JobCard(const Job &job, bool isRecruiter, QWidget *parent = nullptr);
    long getJobId() const { return jobData.getId(); }

signals:
    void applyClicked(long jobId);
    void deleteClicked(long jobId);

private:
    Job jobData;
};


#endif //JOBCARD_H
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <vector>
#include "Job.h"

class MainWindow : public QMainWindow {
    Q_OBJECT // required Macro for QT signals
public:
    explicit MainWindow(QWidget *parent = nullptr); // constructor
    ~MainWindow(); // destructor
private slots:
    void loadJobs(); // trigger by Refresh button
    void deleteSelectedJobs(); // trigger by Delete button
private:
    QTableWidget *tableWidget;
    QPushButton *refreshButton;
    QPushButton *deleteButton;

    QNetworkAccessManager *networkAccessManager; // handle HTTP requests from Spring backend
    const QString API_URL = "http://localhost:8080/api/jobs";
    const QString AUTH_TOKEN = "eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJzaWtpdzIzMDY2QG9rY2RlYWxzLmNvbSIsImlhdCI6MTc2NDA5NDA5MSwiZXhwIjoxNzY0MTgwNDkxfQ.FN6uG-mXonOCnGil_eWudvz-oLVnLA-qXoOiyiaOO9M";

    std::vector<Job> jobsList;
    void updateTable();
};

#endif //MAINWINDOW_H
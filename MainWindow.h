#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <vector>

#include "AuthManager.h"
#include "Job.h"

class MainWindow : public QMainWindow {
    Q_OBJECT // required Macro for QT signals
public:
    explicit MainWindow(QWidget *parent = nullptr); // constructor
    ~MainWindow(); // destructor
private slots:
    void loadJobs(); // trigger by Refresh button
    void deleteSelectedJobs(); // trigger by Delete button
    void openLogin();
    void applyForJob();
private:
    QTableWidget *tableWidget;
    QPushButton *refreshButton;
    QPushButton *deleteButton;
    AuthManager *authManager;
    QPushButton *loginButton;
    QPushButton *applyButton;

    QNetworkAccessManager *networkAccessManager; // handle HTTP requests from Spring backend
    const QString API_URL = "http://localhost:8080/api/jobs";
    const QString AUTH_TOKEN;

    std::vector<Job> jobsList;
    void updateTable();
};

#endif //MAINWINDOW_H
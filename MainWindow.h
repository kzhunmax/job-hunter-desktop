#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
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
    void openLogin();
    void switchRole();
    void onCardApplyClicked(long jobId);
    void onCardDeleteClicked(long jobId);
private:
    QScrollArea *scrollArea;
    QWidget *cardsContainer;
    QVBoxLayout *cardsLayout;
    QPushButton *refreshButton;
    QPushButton *deleteButton;
    QPushButton *loginButton;
    QPushButton *applyButton;
    QPushButton *switchRoleButton;

    AuthManager *authManager;
    QNetworkAccessManager *networkAccessManager; // handle HTTP requests from Spring backend
    const QString API_URL = "http://localhost:8080/api/jobs";
    const QString APPLICATION_URL = "http://localhost:8080/api/applications";

    std::vector<Job> jobsList;
    void renderJobs();
};

#endif //MAINWINDOW_H
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScrollArea>
#include <vector>
#include "AuthManager.h"
#include "Job.h"
#include "Toast.h"

class MainWindow : public QMainWindow {
    Q_OBJECT // required Macro for QT signals
public:
    explicit MainWindow(QWidget *parent = nullptr); // constructor
    ~MainWindow() override; // destructor
private slots:
    void loadJobs(); // trigger by Refresh button
    void openLogin();
    void switchRole();
    void toggleTheme();
    void onCardApplyClicked(long jobId);
    void onCardDeleteClicked(long jobId);
private:
    QScrollArea *scrollArea;
    QWidget *centralWidget;
    QWidget *cardsContainer;
    QVBoxLayout *cardsLayout;
    QWidget *topBar;
    QLabel *logoLabel;

    QPushButton *refreshButton;
    QPushButton *loginButton;
    QPushButton *switchRoleButton;
    QPushButton *themeButton;

    AuthManager *authManager;
    QNetworkAccessManager *networkAccessManager; // handle HTTP requests from Spring backend
    Toast *toast;

    const QString API_URL = "http://localhost:8080/api/jobs";
    const QString APPLICATION_URL = "http://localhost:8080/api/applications";

    std::vector<Job> jobsList;
    bool isDarkMode = false;

    void renderJobs();
    void applyGlobalTheme();
};

#endif //MAINWINDOW_H
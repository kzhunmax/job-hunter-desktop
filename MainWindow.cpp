#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include  <QLabel>

#include "LoginDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    authManager = new AuthManager(this);

    // Setup UI
    const auto centalWidget = new QWidget(this);
    setCentralWidget(centalWidget);
    const auto layout = new QVBoxLayout(centalWidget);

    // Top Bar for login
    auto *topBar = new QWidget;
    auto *topLayout = new QHBoxLayout(topBar);
    layout->addWidget(topBar);

    loginButton = new QPushButton("Login", this);
    const auto switchRoleButton = new QPushButton("Switch Role");
    switchRoleButton->setVisible(false);
    topLayout->addWidget(new QLabel("JobHunter"));
    topLayout->addWidget(switchRoleButton);
    topLayout->addWidget(loginButton);
    topLayout->addStretch();

    // Setup Tables
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4); // ID, Title, Company, Salary
    tableWidget->setHorizontalHeaderLabels({"ID", "Title", "Company", "Salary"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tableWidget);

    // Buttons
    refreshButton = new QPushButton("Refresh", this);
    deleteButton = new QPushButton("Delete", this);
    applyButton = new QPushButton("Apply", this);
    layout->addWidget(applyButton);
    layout->addWidget(refreshButton);
    layout->addWidget(deleteButton);

    // Network Setup
    networkAccessManager = new QNetworkAccessManager(this);

    // Connect Buttons to functions
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::loadJobs);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedJobs);
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::openLogin);
    connect(applyButton, &QPushButton::clicked, this, &MainWindow::applyForJob);
    connect(switchRoleButton, &QPushButton::clicked, authManager, &AuthManager::switchRole);

    // Load on startup
    loadJobs();

    connect(authManager, &AuthManager::loginSuccess, [this, switchRoleButton]() {
        loginButton->hide();
        switchRoleButton->show();
        loadJobs();
    });

    connect(authManager, &AuthManager::roleSwitched, [this]() {
        QMessageBox::information(this, "Success", "Role Switched!");
        loadJobs();
    });
}

MainWindow::~MainWindow() {
}

void MainWindow::loadJobs() {
    // Create Request
    QNetworkRequest request(API_URL);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Send GET request
    QNetworkReply *reply = networkAccessManager->get(request);

    // Handle Response
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // parse JSON
            QByteArray responseData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            QJsonObject obj = doc.object();

            // Access content from "data" response
            QJsonArray jsonJobs = obj["data"].toObject()["content"].toArray();
            jobsList.clear();

            for (const auto &item: jsonJobs) {
                QJsonObject jsonObj = item.toObject();

                Job job(
                    static_cast<long>(jsonObj["id"].toDouble()),
                    jsonObj["title"].toString(),
                    jsonObj["company"].toString(),
                    jsonObj["salary"].toDouble()
                );
                jobsList.push_back(job);
            }

            updateTable();
        } else {
            QMessageBox::critical(this, "Error", "Failed to load jobs: " + reply->errorString(), QMessageBox::Ok);
        }
        reply->deleteLater();
    });
}

void MainWindow::updateTable() {
    tableWidget->setRowCount(0);

    for (const Job &job: jobsList) {
        int row = tableWidget->rowCount();
        tableWidget->insertRow(row);

        // Set text for each sell
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(job.getId())));
        tableWidget->setItem(row, 1, new QTableWidgetItem(job.getTitle()));
        tableWidget->setItem(row, 2, new QTableWidgetItem(job.getCompanyName()));
        tableWidget->setItem(row, 3, new QTableWidgetItem(job.getSalaryString()));
    }
}

void MainWindow::deleteSelectedJobs() {
    // Check if row is selected
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Warning", "Select job to delete");
        return;
    }

    // Get ID from C++ List
    long jobId = jobsList[currentRow].getId();
    QString deleteUrl = API_URL + "/" + QString::number(jobId);
    QNetworkRequest request{QUrl(deleteUrl)};
    request.setRawHeader("Authorization", ("Bearer " + AUTH_TOKEN).toUtf8());

    QNetworkReply *reply = networkAccessManager->deleteResource(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QMessageBox::information(this, "Success", "Job deleted!");
            loadJobs(); // Reload list
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete" + reply->errorString());
        }
        reply->deleteLater();
    });
}

void MainWindow::openLogin() {
    if (!authManager->isAuthenticated()) {
        LoginDialog dialog(authManager, this);
        dialog.exec();
    }
}

void MainWindow::applyForJob() {
    int row = tableWidget->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Info", "Select job to apply");
        return;
    }
    if (!authManager->isAuthenticated()) {
        int res = QMessageBox::question(this, "Login Required",
                                        "You must be logged in to apply. Do you want to login now?");

        if (res == QMessageBox::Yes) {
            LoginDialog dialog(authManager, this);
            if (dialog.exec() == QDialog::Accepted) {
                QMessageBox::information(this, "Success", "Logged in! Click Apply again.");
            }
        }
        return;
    }

    Job selectedJob = jobsList[row];

    // Create the request to /api/applications/apply/{jobId}
    QString applyUrl = QString("http://localhost:8080/api/applications/apply/%1").arg(selectedJob.getId());
    QNetworkRequest request(applyUrl);

    // NOW we add the token
    request.setRawHeader("Authorization", ("Bearer " + authManager->getToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["resumeId"] = 1; // TODO: You need to fetch user's resume ID first!
    body["coverLetter"] = "I am interested in this job.";
}

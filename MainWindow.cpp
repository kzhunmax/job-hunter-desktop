#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QLabel>

#include "JobCard.h"
#include "LoginDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    authManager = new AuthManager(this);
    networkAccessManager = new QNetworkAccessManager(this);

    // 1. Central Widget & Main Layout
    const auto centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #f4f7f6;");
    setCentralWidget(centralWidget);
    const auto layout = new QVBoxLayout(centralWidget);

    // 2. Top Bar
    auto *topBar = new QWidget;
    topBar->setStyleSheet("background-color: white; border-bottom: 1px solid #ddd;");
    topBar->setFixedHeight(60);
    auto *topLayout = new QHBoxLayout(topBar);

    auto *logo = new QLabel("JobHunter");
    logo->setStyleSheet("font-size: 20px; font-weight: bold; color: #333; margin-left: 10px;");

    refreshButton = new QPushButton("↻ Refresh");
    refreshButton->setCursor(Qt::PointingHandCursor);

    switchRoleButton = new QPushButton("Switch Role");
    switchRoleButton->setVisible(false); // Hidden by default

    loginButton = new QPushButton("Login");
    loginButton->setStyleSheet("background-color: #007bff; color: white; border-radius: 5px; padding: 5px 15px;");

    topLayout->addWidget(logo);
    topLayout->addWidget(refreshButton);
    topLayout->addStretch();
    topLayout->addWidget(switchRoleButton);
    topLayout->addWidget(loginButton);

    layout->addWidget(topBar);

    // 3. Scroll Area for Cards
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    cardsContainer = new QWidget;
    cardsContainer->setStyleSheet("background-color: transparent;");
    cardsLayout = new QVBoxLayout(cardsContainer);
    cardsLayout->setAlignment(Qt::AlignTop); // Stack cards from top
    cardsLayout->setSpacing(15);
    cardsLayout->setContentsMargins(20, 10, 20, 10);

    scrollArea->setWidget(cardsContainer);
    layout->addWidget(scrollArea);

    // 4. Connections
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::loadJobs);
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::openLogin);
    connect(switchRoleButton, &QPushButton::clicked, this, &MainWindow::switchRole);

    connect(authManager, &AuthManager::loginSuccess, [this]() {
        loginButton->hide();
        switchRoleButton->show();
        loadJobs(); // Reload to update card buttons
        QMessageBox::information(this, "Login", "Welcome back!");
    });

    connect(authManager, &AuthManager::roleSwitched, [this]() {
        QMessageBox::information(this, "Success", "Role Switched!");
        loadJobs();
    });

    // Initial Load
    loadJobs();
}

MainWindow::~MainWindow() = default;

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

            renderJobs();
        } else {
            QMessageBox::critical(this, "Error", "Failed to load jobs: " + reply->errorString(), QMessageBox::Ok);
        }
        reply->deleteLater();
    });
}

void MainWindow::renderJobs() {
    // Clear existing cards
    QLayoutItem *item;
    while ((item = cardsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // TODO: Get real role from AuthManager later. For now, toggle logic or default false.
    bool isRecruiter = false;

    for (const Job &job : jobsList) {
        auto *card = new JobCard(job, isRecruiter, this);

        // Connect card signals to MainWindow slots
        connect(card, &JobCard::applyClicked, this, &MainWindow::onCardApplyClicked);
        connect(card, &JobCard::deleteClicked, this, &MainWindow::onCardDeleteClicked);

        cardsLayout->addWidget(card);
    }
}

void MainWindow::onCardDeleteClicked(long jobId) {
    // Implement delete logic similar to apply but using DELETE verb
    QMessageBox::information(this, "TODO", "Delete implementation coming in next step");
}

void MainWindow::openLogin() {
    if (!authManager->isAuthenticated()) {
        LoginDialog dialog(authManager, this);
        dialog.exec();
    }
}

void MainWindow::onCardApplyClicked(long jobId) {
    if (!authManager->isAuthenticated()) {
        QMessageBox::information(this, "Login Required", "Please login to apply.");
        openLogin();
        return;
    }

    long resumeId = authManager->getResumeId();
    if (resumeId == -1) {
        QMessageBox::warning(this, "Profile Incomplete", "We could not find a resume in your profile. Please upload one via the website.");
        return;
    }

    // Prepare Request
    QString applyUrl = QString("%1/apply/%2").arg(APPLICATION_URL).arg(jobId);
    QNetworkRequest request(applyUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + authManager->getToken()).toUtf8());

    // Dynamic Body
    QJsonObject body;
    body["resumeId"] = static_cast<qint64>(resumeId);
    body["coverLetter"] = "Applied via Desktop App"; // You could add an input dialog for this

    QNetworkReply *reply = networkAccessManager->post(request, QJsonDocument(body).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QMessageBox::information(this, "Success", "Application submitted successfully!");
        } else {
            // Basic error handling for now
            QMessageBox::warning(this, "Failed", "Could not apply: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

void MainWindow::switchRole() {
    authManager->switchRole();
}
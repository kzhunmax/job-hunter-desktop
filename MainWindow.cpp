#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QLabel>
#include <QUrlQuery>

#include "JobCard.h"
#include "LoginDialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    authManager = new AuthManager(this);
    networkAccessManager = new QNetworkAccessManager(this);
    toast = new Toast(this);

    // 1. Central Widget & Main Layout
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 2. Top Bar
    topBar = new QWidget;
    topBar->setFixedHeight(70);
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(20, 0, 20, 0);
    topLayout->setSpacing(15);

    refreshButton = new QPushButton("↻");
    refreshButton->setCursor(Qt::PointingHandCursor);
    refreshButton->setFixedSize(40, 40);

    logoLabel = new QLabel("JobHunter");

    switchRoleButton = new QPushButton("Switch Role");
    switchRoleButton->setCursor(Qt::PointingHandCursor);
    switchRoleButton->setFixedHeight(38);
    switchRoleButton->setVisible(false);

    themeButton = new QPushButton("🌙");
    themeButton->setCursor(Qt::PointingHandCursor);
    themeButton->setFixedSize(40, 40);
    themeButton->setToolTip("Toggle Theme");

    loginButton = new QPushButton("Login");
    loginButton->setCursor(Qt::PointingHandCursor);
    loginButton->setFixedSize(100, 38);

    topLayout->addWidget(refreshButton);
    topLayout->addWidget(logoLabel);
    topLayout->addStretch();
    topLayout->addWidget(switchRoleButton);
    topLayout->addWidget(themeButton);
    topLayout->addWidget(loginButton);

    mainLayout->addWidget(topBar);
    setupSearchUI(mainLayout);

    // 3. Scroll Area for Cards
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    cardsContainer = new QWidget;
    cardsLayout = new QVBoxLayout(cardsContainer);
    cardsLayout->setAlignment(Qt::AlignTop);
    cardsLayout->setSpacing(20);
    cardsLayout->setContentsMargins(30, 30, 30, 30);

    scrollArea->setWidget(cardsContainer);
    mainLayout->addWidget(scrollArea);

    // 4. Connections
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::loadJobs);
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::openLogin);
    connect(switchRoleButton, &QPushButton::clicked, this, &MainWindow::switchRole);
    connect(themeButton, &QPushButton::clicked, this, &MainWindow::toggleTheme);

    connect(searchButton, &QPushButton::clicked, this, &MainWindow::searchJobs);
    connect(searchQueryInput, &QLineEdit::returnPressed, this, &MainWindow::searchJobs);
    connect(searchLocationInput, &QLineEdit::returnPressed, this, &MainWindow::searchJobs);

    connect(authManager, &AuthManager::loginSuccess, [this]() {
        loginButton->hide();
        switchRoleButton->show();
        loadJobs();
        toast->showMessage("Welcome back!", Toast::Success);
    });

    connect(authManager, &AuthManager::roleSwitched, [this]() {
        toast->showMessage("Role switched successfully", Toast::Success);
        loadJobs();
    });

    applyGlobalTheme();
    loadJobs();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupSearchUI(QVBoxLayout *mainLayout) {
    searchContainer = new QWidget;
    auto *layout = new QHBoxLayout(searchContainer);
    layout->setContentsMargins(30, 20, 30, 10);
    layout->setSpacing(10);

    searchQueryInput = new QLineEdit;
    searchQueryInput->setPlaceholderText("🔍 Job title, keywords, or company");
    searchQueryInput->setFixedHeight(40);

    searchLocationInput = new QLineEdit;
    searchLocationInput->setPlaceholderText("📍 City, State, or Remote");
    searchLocationInput->setFixedHeight(40);
    searchLocationInput->setFixedWidth(200);

    searchButton = new QPushButton("Search");
    searchButton->setCursor(Qt::PointingHandCursor);
    searchButton->setFixedSize(100, 40);

    layout->addWidget(searchQueryInput);
    layout->addWidget(searchLocationInput);
    layout->addWidget(searchButton);

    mainLayout->addWidget(searchContainer);
}

void MainWindow::searchJobs() {
    QString query = searchQueryInput->text().trimmed();
    QString location = searchLocationInput->text().trimmed();

    // If inputs are empty, revert to standard load
    if (query.isEmpty() && location.isEmpty()) {
        loadJobs();
        return;
    }

    // Build Search URL
    QUrl url(API_URL + "/search");
    QUrlQuery queryParams;
    if (!query.isEmpty()) queryParams.addQueryItem("query", query);
    if (!location.isEmpty()) queryParams.addQueryItem("location", location);
    url.setQuery(queryParams);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if(authManager->isAuthenticated()) {
        request.setRawHeader("Authorization", ("Bearer " + authManager->getToken()).toUtf8());
    }

    QNetworkReply *reply = networkAccessManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(responseData);

            QJsonArray jsonJobs = doc.object()["data"].toObject()["content"].toArray();
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
            toast->showMessage(QString("Found %1 jobs").arg(jobsList.size()), Toast::Success);
        } else {
            toast->showMessage("Search failed: " + reply->errorString(), Toast::Error);
        }
        reply->deleteLater();
    });
}

void MainWindow::toggleTheme() {
    isDarkMode = !isDarkMode;
    themeButton->setText(isDarkMode ? "☀️" : "🌙");
    applyGlobalTheme();
}

void MainWindow::applyGlobalTheme() {
    QString bgColor = isDarkMode ? "#0f172a" : "#f8fafc";
    QString barColor = isDarkMode ? "#1e293b" : "#ffffff";
    QString textColor = isDarkMode ? "#f1f5f9" : "#1e293b";
    QString borderColor = isDarkMode ? "#334155" : "#e2e8f0";
    QString primaryBtn = isDarkMode ? "#3b82f6" : "#2563eb";

    QString scrollStyle = QString(
        "QScrollBar:vertical { border: none; background: %1; width: 10px; margin: 0px; }"
        "QScrollBar::handle:vertical { background: %2; min-height: 20px; border-radius: 5px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }"
    ).arg(bgColor, isDarkMode ? "#475569" : "#cbd5e1");

    centralWidget->setStyleSheet("background-color: " + bgColor + ";");
    cardsContainer->setStyleSheet("background-color: " + bgColor + ";");
    scrollArea->setStyleSheet("background-color: " + bgColor + "; border: none;");

    topBar->setStyleSheet(QString("background-color: %1; border-bottom: 1px solid %2;").arg(barColor, borderColor));
    logoLabel->setStyleSheet(QString("color: %1; font-size: 24px; font-weight: 800; font-family: 'Segoe UI', sans-serif;").arg(textColor));

    QString iconBtnStyle = QString(
        "QPushButton { background-color: transparent; color: %1; border-radius: 20px; font-size: 18px; font-weight: bold; }"
        "QPushButton:hover { background-color: %2; }"
    ).arg(textColor, isDarkMode ? "#334155" : "#e2e8f0");

    refreshButton->setStyleSheet(iconBtnStyle);
    themeButton->setStyleSheet(iconBtnStyle);

    QString stdBtnStyle = QString(
        "QPushButton { background-color: transparent; color: %1; border: 1px solid %2; border-radius: 6px; font-weight: 600; padding: 5px 10px; }"
        "QPushButton:hover { background-color: %3; }"
    ).arg(textColor, borderColor, isDarkMode ? "#334155" : "#f1f5f9");

    switchRoleButton->setStyleSheet(stdBtnStyle);

    loginButton->setStyleSheet(QString(
        "QPushButton { background-color: %1; color: white; border: none; border-radius: 6px; font-weight: 700; }"
        "QPushButton:hover { opacity: 0.9; }"
    ).arg(primaryBtn));

    QString inputStyle = QString(
        "QLineEdit { border: 1px solid %1; border-radius: 8px; padding: 0 12px; background: %2; color: %3; font-size: 14px; }"
        "QLineEdit:focus { border: 2px solid #3b82f6; }"
    ).arg(isDarkMode ? "#334155" : "#cbd5e1",
          isDarkMode ? "#1e293b" : "#ffffff",
          isDarkMode ? "#f1f5f9" : "#1e293b");

    searchQueryInput->setStyleSheet(inputStyle);
    searchLocationInput->setStyleSheet(inputStyle);

    searchButton->setStyleSheet(
        "QPushButton { background-color: #3b82f6; color: white; border-radius: 8px; font-weight: bold; font-size: 14px; }"
        "QPushButton:hover { background-color: #2563eb; }"
    );

    if(toast) toast->setParent(this);
    renderJobs();
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

            renderJobs();
        } else {
            toast->showMessage("Failed to load jobs: " + reply->errorString(), Toast::Error);
        }
        reply->deleteLater();
    });
}

void MainWindow::renderJobs() {
    QLayoutItem *item;
    while ((item = cardsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    bool isRecruiter = authManager->isRecruiter();

    for (const Job &job: jobsList) {
        auto *card = new JobCard(job, isRecruiter, this);

        card->applyTheme(isDarkMode);

        connect(card, &JobCard::applyClicked, this, &MainWindow::onCardApplyClicked);
        connect(card, &JobCard::deleteClicked, this, &MainWindow::onCardDeleteClicked);

        cardsLayout->addWidget(card);
    }
}

void MainWindow::onCardDeleteClicked(long jobId) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this job?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return;

    QString deleteUrl = QString("%1/%2").arg(API_URL).arg(jobId);
    QNetworkRequest request((QUrl(deleteUrl)));
    request.setRawHeader("Authorization", ("Bearer " + authManager->getToken()).toUtf8());

    QNetworkReply *netReply = networkAccessManager->deleteResource(request);

    connect(netReply, &QNetworkReply::finished, [this, netReply]() {
        if (netReply->error() == QNetworkReply::NoError) {
            toast->showMessage("Job deleted successfully", Toast::Success);
            loadJobs();
        } else {
            toast->showMessage("Failed to delete job " + netReply->errorString(), Toast::Error);
        }
        netReply->deleteLater();
    });
}

void MainWindow::openLogin() {
    if (!authManager->isAuthenticated()) {
        LoginDialog dialog(authManager, this);
        dialog.updateTheme(isDarkMode);
        dialog.exec();
    }
}

void MainWindow::onCardApplyClicked(long jobId) {
    if (!authManager->isAuthenticated()) {
        toast->showMessage("Please login to apply", Toast::Info);
        openLogin();
        return;
    }

    long resumeId = authManager->getResumeId();
    if (resumeId == -1) {
        toast->showMessage("We could not find a resume in your profile", Toast::Error);
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
    body["coverLetter"] = "Applied via Desktop App";

    QNetworkReply *reply = networkAccessManager->post(request, QJsonDocument(body).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            toast->showMessage("Application submitted successfully!", Toast::Success);
        } else {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QString msg = "Application failed.";
            if (doc.isObject() && doc.object().contains("errors")) {
                msg = doc.object()["errors"].toArray().at(0).toObject()["message"].toString();
            }
            toast->showMessage("Application Failed: " + msg, Toast::Error);
        }
        reply->deleteLater();
    });
}

void MainWindow::switchRole() {
    authManager->switchRole();
}
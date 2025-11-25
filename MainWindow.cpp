#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Setup UI
    const auto centalWidget = new QWidget(this);
    setCentralWidget(centalWidget);
    const auto layout = new QVBoxLayout(centalWidget);

    // Setup Tables
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(4); // ID, Title, Company, Salary
    tableWidget->setHorizontalHeaderLabels({"ID", "Title", "Company", "Salary"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(tableWidget);

    // Buttons
    refreshButton = new QPushButton("Refresh", this);
    deleteButton = new QPushButton("Delete", this);
    layout->addWidget(refreshButton);
    layout->addWidget(deleteButton);

    // Network Setup
    networkAccessManager = new QNetworkAccessManager(this);

    // Connect Buttons to functions
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::loadJobs);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedJobs);

    // Load on startup
    loadJobs();
}

MainWindow::~MainWindow() {}

void MainWindow::loadJobs() {
    // Create Request
    QNetworkRequest request(API_URL);
    request.setRawHeader("Authorization", ("Bearer " + AUTH_TOKEN).toUtf8());
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

            for (const auto &item : jsonJobs) {
                QJsonObject jsonObj = item.toObject();

                Job job (
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

    for (const Job &job : jobsList) {
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
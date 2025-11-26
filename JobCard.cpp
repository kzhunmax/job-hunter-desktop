#include "JobCard.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

JobCard::JobCard(const Job &job, bool isRecruiter, QWidget *parent) : QWidget(parent), jobData(job) {
    // Styling
    this->setStyleSheet(
        "JobCard { background-color: white; border-radius: 10px; border: 1px solid #e0e0e0; }"
        "JobCard:hover { border: 1px solid #b0b0b0; }"
        "QLabel { color: #333; font-family: 'Segoe UI', sans-serif; }"
        "QPushButton { background-color: #007bff; color: white; border: none; border-radius: 5px; padding: 5px 10px; font-weight: bold; }"
        "QPushButton:hover { background-color: #0056b3; }"
        "QPushButton#deleteBtn { background-color: #dc3545; }"
        "QPushButton#deleteBtn:hover { background-color: #c82333; }"
    );
    this->setAttribute(Qt::WA_StyledBackground, true);

    // Main Layout
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Title
    auto *titleLabel = new QLabel(job.getTitle());
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);

    // Company
    auto *companyLabel = new QLabel(job.getCompanyName());
    companyLabel->setStyleSheet("font-size: 14px; color: #6c757d; margin-bottom: 5px;");
    mainLayout->addWidget(companyLabel);

    auto *salaryLabel = new QLabel("💰 " + job.getSalaryString());
    salaryLabel->setStyleSheet("font-size: 14px; color: #28a745; font-weight: 600;");
    mainLayout->addWidget(salaryLabel);

    mainLayout->addStretch();

    // Action Buttons
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(); // Push Buttons to right

    if (isRecruiter) {
        auto *delBtn = new QPushButton("Delete");
        delBtn->setObjectName("deleteBtn");
        delBtn->setCursor(Qt::PointingHandCursor);
        connect(delBtn, &QPushButton::clicked, this, [this](){ emit deleteClicked(jobData.getId()); });
        buttonLayout->addWidget(delBtn);
    } else {
        auto *applyBtn = new QPushButton("Apply Now");
        applyBtn->setCursor(Qt::PointingHandCursor);
        connect(applyBtn, &QPushButton::clicked, this, [this](){ emit applyClicked(jobData.getId()); });
        buttonLayout->addWidget(applyBtn);
    }

    mainLayout->addLayout(buttonLayout);

    this->setMinimumHeight(150);
}

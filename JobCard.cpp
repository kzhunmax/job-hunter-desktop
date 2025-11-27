#include "JobCard.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

JobCard::JobCard(const Job &job, bool isRecruiter, QWidget *parent) : QWidget(parent), jobData(job), m_isRecruiter(isRecruiter) {
    this->setAttribute(Qt::WA_StyledBackground, true);

    // Main Layout
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(8);

    // Title
    auto *titleLabel = new QLabel(job.getTitle());
    titleLabel->setObjectName("cardTitle");
    titleLabel->setWordWrap(true);
    mainLayout->addWidget(titleLabel);

    // Company
    auto *companyLabel = new QLabel(job.getCompanyName());
    companyLabel->setObjectName("cardCompany");
    mainLayout->addWidget(companyLabel);

    auto *salaryLabel = new QLabel("💰 " + job.getSalaryString());
    salaryLabel->setObjectName("cardSalary");
    mainLayout->addWidget(salaryLabel);

    mainLayout->addStretch();

    // Action Buttons
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();

    if (isRecruiter) {
        auto *delBtn = new QPushButton("Delete");
        delBtn->setObjectName("deleteBtn");
        delBtn->setCursor(Qt::PointingHandCursor);
        connect(delBtn, &QPushButton::clicked, this, [this](){ emit deleteClicked(jobData.getId()); });
        buttonLayout->addWidget(delBtn);
    } else {
        auto *applyBtn = new QPushButton("Apply Now");
        applyBtn->setObjectName("applyBtn");
        applyBtn->setCursor(Qt::PointingHandCursor);
        connect(applyBtn, &QPushButton::clicked, this, [this](){ emit applyClicked(jobData.getId()); });
        buttonLayout->addWidget(applyBtn);
    }

    mainLayout->addLayout(buttonLayout);
    this->setFixedHeight(200);
}

void JobCard::applyTheme(bool isDark) {
    if (isDark) {
        this->setStyleSheet(
            "JobCard { background-color: #1e293b; border: 1px solid #334155; border-radius: 10px; }"
            "QLabel { background-color: transparent; border: none; }"
            "QLabel#cardTitle { font-size: 18px; font-weight: bold; color: #f1f5f9; }"
            "QLabel#cardCompany { font-size: 14px; font-weight: 600; color: #94a3b8; }"
            "QLabel#cardSalary { font-size: 14px; font-weight: bold; color: #34d399; }"
            "QPushButton#applyBtn { background-color: #3b82f6; color: white; border-radius: 6px; padding: 6px 12px; }"
            "QPushButton#applyBtn:hover { background-color: #2563eb; }"
            "QPushButton#deleteBtn { background-color: #ef4444; color: white; border-radius: 6px; padding: 6px 12px; }"
            "QPushButton#deleteBtn:hover { background-color: #dc2626; }"
        );
    } else {
        this->setStyleSheet(
            "JobCard { background-color: #ffffff; border: 1px solid #e2e8f0; border-radius: 10px; }"
            "QLabel { background-color: transparent; border: none; }"
            "QLabel#cardTitle { font-size: 18px; font-weight: bold; color: #1e293b; }"
            "QLabel#cardCompany { font-size: 14px; font-weight: 600; color: #64748b; }"
            "QLabel#cardSalary { font-size: 14px; font-weight: bold; color: #059669; }"
            "QPushButton#applyBtn { background-color: #2563eb; color: white; border-radius: 6px; padding: 6px 12px; }"
            "QPushButton#applyBtn:hover { background-color: #1d4ed8; }"
            "QPushButton#deleteBtn { background-color: #dc2626; color: white; border-radius: 6px; padding: 6px 12px; }"
            "QPushButton#deleteBtn:hover { background-color: #b91c1c; }"
        );
    }
}

#include "LoginDialog.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

LoginDialog::LoginDialog(AuthManager *auth, QWidget *parent) : QDialog(parent), authManager(auth) {
    setWindowTitle("Access JobHunter");
    setMinimumWidth(400);
    setMaximumWidth(400);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    tabWidget = new QTabWidget(this);
    tabWidget->addTab(createLoginTab(), "Sign In");
    tabWidget->addTab(createRegisterTab(), "Register");
    layout->addWidget(tabWidget);

    updateTheme(false);
}

void LoginDialog::updateTheme(bool isDark) {
    QString bg = isDark ? "#1e293b" : "#ffffff";
    QString text = isDark ? "#f1f5f9" : "#334155";
    QString inputBg = isDark ? "#334155" : "#f8fafc";
    QString border = isDark ? "#475569" : "#cbd5e1";
    QString header = isDark ? "#f8fafc" : "#1e293b";
    QString tabBg = isDark ? "#0f172a" : "#f1f5f9";
    QString comboBg = isDark ? "#334155" : "#ffffff";

    QString style = QString(
        "QDialog { background-color: %1; }"
        "QLabel { font-family: 'Segoe UI'; font-size: 14px; color: %2; }"
        "QLabel#header { font-size: 20px; font-weight: bold; color: %5; margin-bottom: 10px; }"

        // Input Fields
        "QLineEdit { border: 1px solid %4; border-radius: 6px; padding: 8px; background: %3; color: %2; }"
        "QLineEdit:focus { border: 2px solid #3b82f6; }"

        // Buttons
        "QPushButton { background-color: #2563eb; color: white; border-radius: 6px; padding: 10px; font-weight: bold; }"
        "QPushButton:hover { background-color: #1d4ed8; }"
        "QPushButton:disabled { background-color: #94a3b8; }"

        // Tabs
        "QTabWidget::pane { border: none; }"
        "QTabBar::tab { background: %6; color: #64748b; padding: 12px 20px; border: none; width: 160px; }"
        "QTabBar::tab:selected { background: %1; color: #2563eb; border-bottom: 2px solid #2563eb; font-weight: bold; }"

        // ComboBox FIX
        "QComboBox { border: 1px solid %4; border-radius: 6px; padding: 6px; background: %7; color: %2; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background: %7; color: %2; selection-background-color: #2563eb; }"
    ).arg(bg, text, inputBg, border, header, tabBg, comboBg);

    this->setStyleSheet(style);
}

QWidget *LoginDialog::createLoginTab() {
    QWidget *tab = new QWidget;
    auto *layout = new QVBoxLayout(tab);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);

    auto *header = new QLabel("Welcome Back");
    header->setObjectName("header");
    header->setAlignment(Qt::AlignCenter);

    loginEmail = new QLineEdit;
    loginEmail->setPlaceholderText("email@example.com");

    loginPassword = new QLineEdit;
    loginPassword->setPlaceholderText("••••••••");
    loginPassword->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Sign In");

    layout->addWidget(header);
    layout->addWidget(new QLabel("Email Address"));
    layout->addWidget(loginEmail);
    layout->addWidget(new QLabel("Password"));
    layout->addWidget(loginPassword);
    layout->addSpacing(10);
    layout->addWidget(loginButton);
    layout->addStretch();

    // Logic
    connect(loginButton, &QPushButton::clicked, [this]() {
        loginButton->setEnabled(false);
        loginButton->setText("Signing in...");
        authManager->login(loginEmail->text(), loginPassword->text());
    });

    connect(authManager, &AuthManager::loginSuccess, this, &QDialog::accept);

    connect(authManager, &AuthManager::loginFailed, [this](QString msg) {
        QMessageBox::warning(this, "Login Failed", msg);
        loginButton->setEnabled(true);
        loginButton->setText("Sign In");
    });
    return tab;
}

QWidget *LoginDialog::createRegisterTab() {
    QWidget *tab = new QWidget;
    auto *layout = new QVBoxLayout(tab);
    layout->setSpacing(12);
    layout->setContentsMargins(30, 30, 30, 30);

    auto *header = new QLabel("Create Account");
    header->setObjectName("header");
    header->setAlignment(Qt::AlignCenter);

    regEmail = new QLineEdit; regEmail->setPlaceholderText("Email");
    regPassword = new QLineEdit; regPassword->setPlaceholderText("Password"); regPassword->setEchoMode(QLineEdit::Password);
    regConfirm = new QLineEdit; regConfirm->setPlaceholderText("Confirm Password"); regConfirm->setEchoMode(QLineEdit::Password);

    roleCombo = new QComboBox;
    roleCombo->addItem("I am a Candidate", "ROLE_CANDIDATE");
    roleCombo->addItem("I am a Recruiter", "ROLE_RECRUITER");
    roleCombo->setStyleSheet("padding: 8px; border-radius: 6px; border: 1px solid #cbd5e1;");

    QPushButton *registerButton = new QPushButton("Create Account");

    layout->addWidget(header);
    layout->addWidget(regEmail);
    layout->addWidget(regPassword);
    layout->addWidget(regConfirm);
    layout->addWidget(new QLabel("I want to be a:"));
    layout->addWidget(roleCombo);
    layout->addSpacing(10);
    layout->addWidget(registerButton);
    layout->addStretch();

    connect(registerButton, &QPushButton::clicked, [this, registerButton]() {
        QString role = roleCombo->currentData().toString();
        registerButton->setEnabled(false);
        authManager->registerUser(regEmail->text(), regPassword->text(), regConfirm->text(), role);
    });

    connect(authManager, &AuthManager::registrationSuccess, [this]() {
        QMessageBox::information(this, "Success", "Account created! Please log in.");
        tabWidget->setCurrentIndex(0);
    });
    connect(authManager, &AuthManager::registrationFailed, [this, registerButton](QString msg) {
            QMessageBox::warning(this, "Registration Failed", msg);
            registerButton->setEnabled(true);
        });
    return tab;
}

#include "LoginDialog.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

LoginDialog::LoginDialog(AuthManager *auth, QWidget *parent) : QDialog(parent), authManager(auth) {
    setWindowTitle("Welcome to JobHunter");
    setMinimumWidth(300);

    auto layout = new QVBoxLayout(this);
    tabWidget = new QTabWidget(this);

    tabWidget->addTab(createLoginTab(), "Login");
    tabWidget->addTab(createRegisterTab(), "Register");
    layout->addWidget(tabWidget);
}

QWidget *LoginDialog::createLoginTab() {
    QWidget *tab = new QWidget;
    auto *layout = new QVBoxLayout(tab);


    loginEmail = new QLineEdit;
    loginEmail->setPlaceholderText("Email");
    loginPassword = new QLineEdit;
    loginPassword->setPlaceholderText("Password");
    loginPassword->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Login");

    layout->addWidget(new QLabel("Sign in with your account"));
    layout->addWidget(loginEmail);
    layout->addWidget(loginPassword);
    layout->addWidget(loginButton);
    layout->addStretch();

    // Logic
    connect(loginButton, &QPushButton::clicked, [this]() {
        loginButton->setEnabled(false);
        authManager->login(loginEmail->text(), loginPassword->text());
    });

    // Close dialog on success
    connect(authManager, &AuthManager::loginSuccess, this, &QDialog::accept);

    connect(authManager, &AuthManager::loginFailed, [this] {
        QMessageBox::warning(this, "Warning", "Failed to login");
        loginButton->setEnabled(true);
    });
    return tab;
}

QWidget *LoginDialog::createRegisterTab() {
    QWidget *tab = new QWidget;
    auto *layout = new QVBoxLayout(tab);

    regEmail = new QLineEdit;
    regEmail->setPlaceholderText("Email");
    regPassword = new QLineEdit;
    regPassword->setPlaceholderText("Password");
    regPassword->setEchoMode(QLineEdit::Password);
    regConfirm = new QLineEdit;
    regConfirm->setPlaceholderText("Confirm Password");
    regConfirm->setEchoMode(QLineEdit::Password);

    roleCombo = new QComboBox;
    roleCombo->addItem("I am a Candidate", "ROLE_CANDIDATE");
    roleCombo->addItem("I am a Recruiter", "ROLE_RECRUITER");

    QPushButton *registerButton = new QPushButton("Create Account");

    layout->addWidget(new QLabel("Join us today!"));
    layout->addWidget(regEmail);
    layout->addWidget(regPassword);
    layout->addWidget(regConfirm);
    layout->addWidget(new QLabel("Select Role:"));
    layout->addWidget(roleCombo);
    layout->addWidget(registerButton);
    layout->addStretch();

    connect(registerButton, &QPushButton::clicked, [this]() {
        QString role = roleCombo->currentData().toString();
        authManager->registerUser(regEmail->text(), regPassword->text(), regConfirm->text(), role);
    });

    connect(authManager, &AuthManager::registrationSuccess, this, &QDialog::accept);

    connect(authManager, &AuthManager::registrationFailed, [this] {
        QMessageBox::warning(this, "Warning", "Failed to register");
        loginButton->setEnabled(true);
    });
    return tab;
}

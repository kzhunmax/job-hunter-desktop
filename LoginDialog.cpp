#include "LoginDialog.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

LoginDialog::LoginDialog(AuthManager *auth, QWidget *parent) : QDialog(parent), authManager(auth) {
    setWindowTitle("Login Required");
    auto layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Email:"));
    emailInput = new QLineEdit(this);
    layout->addWidget(emailInput);

    layout->addWidget(new QLabel("Password:"));
    passwordInput = new QLineEdit(this);
    passwordInput->setEchoMode(QLineEdit::Password);
    layout->addWidget(passwordInput);

    loginButton = new QPushButton("Login", this);
    layout->addWidget(loginButton);

    // Logic
    connect(loginButton, &QPushButton::clicked, [this]() {
        loginButton->setEnabled(false);
        authManager->login(emailInput->text(), passwordInput->text());
    });

    // Close dialog on success
    connect(authManager, &AuthManager::loginSuccess, this, &QDialog::accept);

    // Re-enable button on failure
    connect(authManager, &AuthManager::loginFailed, [this](QString message) {
        loginButton->setEnabled(true);
    });
}

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "AuthManager.h"
#include "MainWindow.h"


class LoginDialog : public QDialog {
    Q_OBJECT;
public:
    explicit LoginDialog(AuthManager *auth, QWidget *parent = nullptr);
private:
    QLineEdit *emailInput;
    QLineEdit *passwordInput;
    QPushButton *loginButton;
    AuthManager *authManager;
};


#endif //LOGINDIALOG_H
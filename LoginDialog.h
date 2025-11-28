#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QTabWidget>
#include "AuthManager.h"
#include "MainWindow.h"


class LoginDialog : public QDialog {
    Q_OBJECT;
public:
    explicit LoginDialog(AuthManager *auth, QWidget *parent = nullptr);
    void updateTheme(bool isDark);
private:
    QTabWidget *tabWidget;

    // Login Widgets
    QLineEdit *loginEmail;
    QLineEdit *loginPassword;
    QPushButton *loginButton;

    // Register Widgets
    QLineEdit *regEmail;
    QLineEdit *regPassword;
    QLineEdit *regConfirm;
    QComboBox *roleCombo;

    AuthManager *authManager;
    Toast *toast;

    QWidget* createLoginTab();
    QWidget* createRegisterTab();
};


#endif //LOGINDIALOG_H
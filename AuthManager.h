#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H
#include <QNetworkAccessManager>
#include <QObject>


class AuthManager : public QObject {
    Q_OBJECT;
public:
    explicit AuthManager(QObject *parent = nullptr);
    void login(const QString &email, const QString &password);
    void logout();
    QString getToken() const;
    bool isAuthenticated() const;
signals:
    void loginSuccess();
    void loginFailed(QString message);
    void authStateChanged(bool isLoggedIn);
private:
    QNetworkAccessManager *networkManager;
    QString accessToken;
    const QString LOGIN_URL = "http://localhost:8080/api/auth/login";
};


#endif //AUTHMANAGER_H
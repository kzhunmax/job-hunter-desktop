#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H
#include <QNetworkAccessManager>
#include <QObject>


class AuthManager : public QObject {
    Q_OBJECT;
public:
    explicit AuthManager(QObject *parent = nullptr);
    void login(const QString &email, const QString &password);
    QString getToken() const;
    bool isAuthenticated() const;
    void registerUser(const QString &email, const QString &password, const QString &confirmPassword, const QString &role);
    void switchRole();
signals:
    void loginSuccess();
    void loginFailed(QString message);
    void registrationSuccess();
    void registrationFailed(QString message);
    void roleSwitched();
private:
    QNetworkAccessManager *networkManager;
    QString accessToken;
    const QString LOGIN_URL = "http://localhost:8080/api/auth/login";
    const QString REGISTER_URL = "http://localhost:8080/api/auth/register";
    const QString SWITCH_ROLE_URL = "http://localhost:8080/api/auth/switch-role";
};


#endif //AUTHMANAGER_H
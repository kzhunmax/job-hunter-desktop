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
    long getResumeId() const;
    bool isRecruiter() const;
signals:
    void loginSuccess();
    void loginFailed(QString message);
    void registrationSuccess();
    void registrationFailed(QString message);
    void roleSwitched();
private:
    QNetworkAccessManager *networkManager;
    QString accessToken;
    long resumeId = -1;
    bool m_isRecruiter = false;

    const QString BASE_URL = "http://localhost:8080/api";

    void fetchUserProfile();
    QString getErrorMessage(QNetworkReply *reply);
};


#endif //AUTHMANAGER_H
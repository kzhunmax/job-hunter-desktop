#include "AuthManager.h"

#include <QJsonObject>
#include <QNetworkReply>
#include <QJsonArray>

AuthManager::AuthManager(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
}

void AuthManager::login(const QString &email, const QString &password) {
    QNetworkRequest request(LOGIN_URL);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply] {
        if (reply->error() == QNetworkReply::NoError) {
            auto doc = QJsonDocument::fromJson(reply->readAll());
            accessToken = doc.object()["data"].toObject()["accessToken"].toString();
            emit loginSuccess();
        } else {
            emit loginFailed("Login failed: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

bool AuthManager::isAuthenticated() const {
    return !accessToken.isEmpty();
}

QString AuthManager::getToken() const {
    return accessToken;
}

void AuthManager::registerUser(const QString &email, const QString &password, const QString &confirmPassword, const QString &role) {
    QNetworkRequest request(REGISTER_URL);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;
    json["confirmPassword"] = confirmPassword;
    json["roles"] = QJsonArray({role});

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply] {
        if (reply->error() == QNetworkReply::NoError) {
            emit registrationSuccess();
        } else {
            emit registrationFailed("Registration failed: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

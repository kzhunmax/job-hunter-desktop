#include "AuthManager.h"

#include <QJsonObject>
#include <QNetworkReply>
#include <QJsonArray>

AuthManager::AuthManager(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
}

void AuthManager::login(const QString &email, const QString &password) {
    QNetworkRequest request(QUrl(BASE_URL + "/auth/login"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    QNetworkReply *reply = networkManager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, [this, reply] {
        if (reply->error() == QNetworkReply::NoError) {
            auto doc = QJsonDocument::fromJson(reply->readAll());
            accessToken = doc.object()["data"].toObject()["accessToken"].toString();
            fetchUserProfile();
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
    QNetworkRequest request(QUrl(BASE_URL + "/auth/register"));
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

void AuthManager::switchRole() {
    QNetworkRequest request((BASE_URL + "/auth/switch-role"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + accessToken).toUtf8());

    QNetworkReply *reply = networkManager->post(request, QByteArray()); // Empty body

    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            auto doc = QJsonDocument::fromJson(reply->readAll());
            accessToken = doc.object()["data"].toObject()["accessToken"].toString();
            emit roleSwitched();
        }
        reply->deleteLater();
    });
}

void AuthManager::fetchUserProfile() {
    QNetworkRequest request(QUrl(BASE_URL + "/user/profile"));
    request.setRawHeader("Authorization", ("Bearer " + accessToken).toUtf8());

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, [this, reply] {
        if (reply->error() == QNetworkReply::NoError) {
            auto doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject data = doc.object()["data"].toObject();

            // Extract resumes array
            if (data.contains("resumes") && data["resumes"].isArray()) {
                QJsonArray resumes = data["resumes"].toArray();
                if (!resumes.isEmpty()) {
                    resumeId = static_cast<long>(resumes.at(0).toObject()["id"].toDouble());
                }
            }
            emit loginSuccess();
        } else {
            emit loginSuccess();
        }
        reply->deleteLater();
    });
}

long AuthManager::getResumeId() const {
    return resumeId;
}

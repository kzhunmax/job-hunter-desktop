#include "AuthManager.h"

#include <QJsonObject>
#include <QNetworkReply>
#include <QJsonArray>

AuthManager::AuthManager(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
}

QString AuthManager::getErrorMessage(QNetworkReply *reply) {
    QByteArray data = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    if (!document.isNull() && document.isObject()) {
        QJsonObject object = document.object();
        if (object.contains("errors") && object["errors"].isArray()) {
            QJsonArray errors = object["errors"].toArray();
            if (!errors.isEmpty()) {
                return errors.at(0).toObject()["message"].toString();
            }
        }
    }
    QString err = reply->errorString();
    if (err.contains("Connection refused")) return "Could not connect to server.";
    return err;
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
        } else {
            emit loginFailed(getErrorMessage(reply));
        }
        reply->deleteLater();
    });
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
            emit registrationFailed(getErrorMessage(reply));
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
            fetchUserProfile();
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

            QString type = data["profileType"].toString();
            m_isRecruiter = (type == "RECRUITER");

            if (!m_isRecruiter && data.contains("resumes") && data["resumes"].isArray()) {
                QJsonArray resumes = data["resumes"].toArray();
                if (!resumes.isEmpty()) {
                    resumeId = static_cast<long>(resumes.at(0).toObject()["id"].toDouble());
                }
            }
        }
        emit loginSuccess();
        reply->deleteLater();
    });
}

bool AuthManager::isAuthenticated() const {
    return !accessToken.isEmpty();
}

QString AuthManager::getToken() const {
    return accessToken;
}

long AuthManager::getResumeId() const {
    return resumeId;
}

bool AuthManager::isRecruiter() const {
    return m_isRecruiter;
}
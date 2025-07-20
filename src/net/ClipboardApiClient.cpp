#include "ClipboardApiClient.h"
#include "magic_enum/magic_enum.hpp"
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QJsonDocument>
#include <QJsonObject>

ClipboardApiClient::ClipboardApiClient(const QUrl &baseUrl, QObject *parent)
    : QObject(parent), manager(new QNetworkAccessManager(this)),
      baseUrl(baseUrl) {
  connect(manager, &QNetworkAccessManager::finished, this,
          &ClipboardApiClient::onNetworkReply);
}

ClipboardApiClient::~ClipboardApiClient() = default;

void ClipboardApiClient::registerUser(const QString &email,
                                      const QString &password) {
  QUrl url = baseUrl.resolved(QUrl("/auth/register"));
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject body;
  body["email"] = email;
  body["password"] = password;
  QNetworkReply *reply = manager->post(req, QJsonDocument(body).toJson());
  replyMap.insert(reply, Endpoint::Register);
}

void ClipboardApiClient::login(const User &user) {
  QUrl url = baseUrl.resolved(QUrl("/auth/login"));
  QNetworkRequest req(url);
  req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  QJsonObject body;
  body["email"] = user.email;
  body["password"] = user.password;
  body["device_id"] = user.deviceId;
  body["device_name"] = user.deviceName;
  auto deviceType = magic_enum::enum_name(user.deviceType);
  body["device_type"] = QString::fromStdString(deviceType.data());

  QNetworkReply *reply = manager->post(req, QJsonDocument(body).toJson());
  replyMap.insert(reply, Endpoint::Login);
}

void ClipboardApiClient::uploadClipboard(const ClipboardData &data,
                                         const QString &authToken) {
  QUrl url = baseUrl.resolved(QUrl("/clipboard"));
  QNetworkRequest req(url);
  req.setRawHeader("Authorization",
                   QString("Bearer %1").arg(authToken).toUtf8());

  QJsonObject body;
  body["data"] = data.data;
  auto type = magic_enum::enum_name(data.type);
  body["type"] = QString::fromStdString(type.data());
  body["meta"] = data.meta;

  QNetworkReply *reply = manager->post(req, QJsonDocument(body).toJson());
  replyMap.insert(reply, Endpoint::Upload);
}

void ClipboardApiClient::onNetworkReply(QNetworkReply *reply) {
  Endpoint ep = replyMap.take(reply);

  if (reply->error() != QNetworkReply::NoError) {
    QString err = reply->errorString();
    switch (ep) {
    case Endpoint::Register:
      emit registrationFinished(false, err);
      break;
    case Endpoint::Login:
      emit loginFinished(false, {}, err);
      break;
    case Endpoint::Upload:
      emit uploadFinished(false, err);
      break;
    }
    reply->deleteLater();
    return;
  }

  // 解析 JSON 统一响应格式：{ "code":0, "message":"...", "token":"...", ... }
  handleJsonResponse(reply, ep);
  reply->deleteLater();
}

void ClipboardApiClient::handleJsonResponse(QNetworkReply *reply, Endpoint ep) {
  const auto bytes = reply->readAll();
  const auto doc = QJsonDocument::fromJson(bytes);
  const auto obj = doc.object();

  switch (ep) {
  case Endpoint::Register: {
    bool success = obj.value("code").toInt() == 0;
    QString message = obj.value("message").toString();
    emit registrationFinished(success, message);
  } break;
  case Endpoint::Login: {
    QString accessToken = obj.value("access_token").toString();
    QString refreshToken = obj.value("access_token").toString();
    emit loginFinished(
        true, {.accessToken = accessToken, .refreshToken = refreshToken}, "");
  } break;
  case Endpoint::Upload: {
    QString id = obj.value("id").toString();
    QString created = obj.value("created_at").toString();
    QString source = obj.value("source_device").toString();
    emit uploadFinished(true, bytes);
  }
    break;
  }
}

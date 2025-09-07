#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QUrl>

#include "ClipboardStruct.h"

class ClipboardApiClient : public QObject {
  Q_OBJECT

public:
  explicit ClipboardApiClient(const QUrl &baseUrl, QObject *parent = nullptr);
  ~ClipboardApiClient() override;

  void setUrl(const QUrl &url);

  // 注册新用户
  void registerUser(const QString &email, const QString &password);

  // 登录（成功后可获得访问 token）
  void login(const User &user);

  // 校验token是否有效
  void verifyToken(const QString &authToken);

  // 上传剪贴板数据（二进制），需要携带已登录的 token
  void uploadClipboard(const ClipboardData &data, const QString &authToken);

  // 下载图片
  void downloadImage(const QString &imageUrl, const QString &authToken);

signals:
  void registrationFinished(bool success, const QString &message);
  void loginFinished(bool success, const Token &token, const QString &message);
  void uploadFinished(bool success, const QString &message);
  void imageDownloadFinished(bool success, const QImage &image, const QString &message);
  void verifyTokenFinished(bool success, const QString &message);

private slots:
  // 统一处理所有 reply
  void onNetworkReply(QNetworkReply *reply);

private:
  enum class Endpoint { Register, Login, Upload, DownloadImage, VerifyToken };
  void handleJsonResponse(QNetworkReply *reply, Endpoint ep);
  void handleImageDownload(QNetworkReply *reply);

private:
  QNetworkAccessManager *manager;
  QUrl baseUrl;

  QHash<QNetworkReply *, Endpoint> replyMap;
};

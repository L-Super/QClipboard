#pragma once

#include "ClipboardStruct.h"

#include <QAbstractSocket>
#include <QObject>
#include <QUrl>

class ClipboardApiClient;
class ClipboardWebSocketClient;

class SyncServer : public QObject {
  Q_OBJECT

public:
  explicit SyncServer(const QUrl &apiBaseUrl, QObject *parent = nullptr);
  ~SyncServer() override;

  void setUrl(const QUrl &apiBaseUrl);

  bool isLoggedIn() const;

public slots:
  // HTTP 接口
  void registerUser(const QString &username, const QString &password);
  void login(const User &user);
  void uploadClipboardData(const ClipboardData &data);

  // 手动控制 WebSocket（可选）
  void startSync();
  void stopSync();

signals:
  // HTTP 回调
  void registrationFinished(bool success, const QString &message);
  void loginFinished(bool success, const Token &token, const QString &message);
  void uploadFinished(bool success, const QString &message);

  // 收到 WebSocket message
  void notifyMessageReceived(const QString &message);

  // WebSocket 连接状态
  void syncConnected();
  void syncDisconnected();
  void syncError(QAbstractSocket::SocketError error);

private slots:
  // 处理登录成功后的逻辑：保存 token 并（重新）启动 WebSocket
  void handleLoginFinished(bool success, const Token &token, const QString &message);

private:
  QUrl apiBaseUrl;
  QUrl wsBaseUrl;
  ClipboardApiClient *apiClient{};
  ClipboardWebSocketClient *wsClient{nullptr};
  QString authToken;
  bool isLoginSuccessful{};
};

#pragma once

#include <QObject>
#include <QTimer>
#include <QUrl>
#include <QWebSocket>

class ClipboardWebSocketClient : public QObject {
  Q_OBJECT

public:
  explicit ClipboardWebSocketClient(const QUrl &url, QObject *parent = nullptr);
  ~ClipboardWebSocketClient() override;

  // 发起/断开连接
  void connectToServer();
  void disconnectFromServer();

signals:
  void connected();
  void disconnected();
  void notifyMessageReceived(const QString &message);
  void errorOccurred(QAbstractSocket::SocketError error);

private slots:
  void onConnected();
  void onTextMessageReceived(const QString &message);
  void onBinaryMessageReceived(const QByteArray &message);
  void onError(QAbstractSocket::SocketError error);
  void onDisconnected();
  void tryReconnect();

private:
  QWebSocket webSocket;
  QUrl serverUrl;
  QTimer reconnectTimer;
  const int reconnectIntervalMs = 5000;
};

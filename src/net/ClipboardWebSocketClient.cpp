#include "ClipboardWebSocketClient.h"
#include "../utils/Logger.hpp"
#include <QDebug>

#include <magic_enum/magic_enum.hpp>

ClipboardWebSocketClient::ClipboardWebSocketClient(const QUrl& url, QObject* parent) : QObject(parent), serverUrl(url) {
  // 连接底层信号
  connect(&webSocket, &QWebSocket::connected, this, &ClipboardWebSocketClient::onConnected);
  connect(&webSocket, &QWebSocket::errorOccurred, this, &ClipboardWebSocketClient::onError);
  connect(&webSocket, &QWebSocket::disconnected, this, &ClipboardWebSocketClient::onDisconnected);

  // 重连定时器
  reconnectTimer.setInterval(reconnectIntervalMs);
  reconnectTimer.setSingleShot(true);
  connect(&reconnectTimer, &QTimer::timeout, this, &ClipboardWebSocketClient::tryReconnect);
}

ClipboardWebSocketClient::~ClipboardWebSocketClient() { disconnectFromServer(); }

void ClipboardWebSocketClient::connectToServer() {
  if (webSocket.state() == QAbstractSocket::ConnectedState || webSocket.state() == QAbstractSocket::ConnectingState) {
    return;
  }
  QUrl url(serverUrl);
  url.setQuery(QString()); // 清空查询

  spdlog::info("Connecting to websocket {}", url.toString());
  webSocket.open(serverUrl);
}

void ClipboardWebSocketClient::disconnectFromServer() {
  reconnectTimer.stop();
  webSocket.close();
}

void ClipboardWebSocketClient::onConnected() {
  emit connected();
  spdlog::info("WebSocket connected.");

  // 订阅 /sync/notify（如果服务端需要额外握手可以在这里发送订阅消息）
  // webSocket.sendTextMessage(QStringLiteral("{\"action\":\"subscribe\",\"topic\":\"/sync/notify\"}"));

  // 监听消息
  connect(&webSocket, &QWebSocket::textMessageReceived, this, &ClipboardWebSocketClient::onTextMessageReceived);
  connect(&webSocket, &QWebSocket::binaryMessageReceived, this, &ClipboardWebSocketClient::onBinaryMessageReceived);
}

void ClipboardWebSocketClient::onTextMessageReceived(const QString& message) {
  qDebug() << __func__ << "Websocket text message:" << message;
  emit notifyMessageReceived(message);
}

void ClipboardWebSocketClient::onBinaryMessageReceived(const QByteArray& message) {
  qDebug() << __func__ << "Websocket binary message received; length =" << message.size();
  // 如果服务端以二进制发推送，可以转换成 QString 或 QJsonDocument
  emit notifyMessageReceived(QString::fromUtf8(message));
}

void ClipboardWebSocketClient::onError(QAbstractSocket::SocketError error) {
  emit errorOccurred(error);

  spdlog::error("WebSocket error:{} {}", magic_enum::enum_name(error), webSocket.errorString());

  // 启动重连
  if (!reconnectTimer.isActive())
    reconnectTimer.start();
}

void ClipboardWebSocketClient::onDisconnected() {
  emit disconnected();
  spdlog::warn("WebSocket disconnected.");

  // 尝试重连
  if (!reconnectTimer.isActive())
    reconnectTimer.start();
}

void ClipboardWebSocketClient::tryReconnect() {
  spdlog::warn("Reconnecting...");
  connectToServer();
}

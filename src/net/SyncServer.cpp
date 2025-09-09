#include "SyncServer.h"
#include "ClipboardApiClient.h"
#include "ClipboardWebSocketClient.h"

#include <QEventLoop>
#include <QImage>
#include <QTimer>
#include <QUrlQuery>

SyncServer::SyncServer(const QUrl &apiBaseUrl, QObject *parent)
    : QObject(parent), apiClient(new ClipboardApiClient(apiBaseUrl, this)) {
  setUrl(apiBaseUrl);
  // 绑定 HTTP 客户端信号
  connect(apiClient, &ClipboardApiClient::registrationFinished, this, &SyncServer::registrationFinished);
  connect(apiClient, &ClipboardApiClient::loginFinished, this, &SyncServer::handleLoginFinished);
  connect(apiClient, &ClipboardApiClient::uploadFinished, this, &SyncServer::uploadFinished);
  connect(apiClient, &ClipboardApiClient::imageDownloadFinished, this, &SyncServer::imageDownloadFinished);
}

SyncServer::~SyncServer() {
  stopSync();
}

void SyncServer::setUrl(const QUrl &apiBaseUrl) {
  this->apiBaseUrl = apiBaseUrl;
  apiClient->setUrl(apiBaseUrl);

  wsBaseUrl.setScheme(apiBaseUrl.scheme() == "https" ? "wss" : "ws");
  wsBaseUrl.setHost(apiBaseUrl.host());
  wsBaseUrl.setPort(apiBaseUrl.port());
  wsBaseUrl.setPath("/sync/notify");
}

bool SyncServer::setToken(const QString &token) {
  if (verifyTokenValid(token)) {
    authToken = token;
    handleLoginFinished(true, {.accessToken = token, .refreshToken = ""}, "");
    return true;
  }

  handleLoginFinished(false, {}, "");
  return false;
}

bool SyncServer::isLoggedIn() const { return isLoginSuccessful; }

void SyncServer::registerUser(const QString &username, const QString &password) {
  apiClient->registerUser(username, password);
}

void SyncServer::login(const User &user) { apiClient->login(user); }

void SyncServer::uploadClipboardData(const ClipboardData &data) {
  if (authToken.isEmpty()) {
    emit uploadFinished(false, "Not authenticated");
    return;
  }
  apiClient->uploadClipboard(data, authToken);
}

void SyncServer::downloadImage(const QString &imageUrl) {
  if (authToken.isEmpty()) {
    emit imageDownloadFinished(false, QImage(), "Not authenticated");
    return;
  }
  apiClient->downloadImage(imageUrl, authToken);
}

void SyncServer::startSync() {
  if (wsClient) {
    wsClient->connectToServer();
  }
}

void SyncServer::stopSync() {
  if (wsClient) {
    wsClient->disconnectFromServer();
  }
}

void SyncServer::handleLoginFinished(bool success, const Token &token, const QString &message) {
  // 先把登录结果透传给调用方
  emit loginFinished(success, token, message);
  if (!success) {
    isLoginSuccessful = false;
    return;
  }

  isLoginSuccessful = true;
  // 保存 token
  authToken = token.accessToken;

  // 如果已有旧的 wsClient，断开并删除
  if (wsClient) {
    wsClient->disconnectFromServer();
    delete wsClient;
    wsClient = nullptr;
  }

  // 构造带 token 查询参数的 WebSocket URL
  QUrl url = wsBaseUrl;
  QUrlQuery query;
  query.addQueryItem("token", authToken);
  url.setQuery(query);

  // 新建 WebSocket 客户端，绑定信号
  wsClient = new ClipboardWebSocketClient(url, this);
  connect(wsClient, &ClipboardWebSocketClient::connected, this, &SyncServer::syncConnected);
  connect(wsClient, &ClipboardWebSocketClient::disconnected, this, &SyncServer::syncDisconnected);
  connect(wsClient, &ClipboardWebSocketClient::errorOccurred, this, &SyncServer::syncError);
  connect(wsClient, &ClipboardWebSocketClient::notifyMessageReceived, this, &SyncServer::notifyMessageReceived);

  // 自动建立连接
  wsClient->connectToServer();
}

bool SyncServer::verifyTokenValid(const QString &token) {
  // 发起 /auth/verify-token 请求，等待结果（带超时）
  QEventLoop loop;
  QTimer timer;
  timer.setSingleShot(true);
  bool ok = false;
  QString msg;

  auto onFinished = [&](bool success, const QString &message) {
    ok = success;
    msg = message;
    if (loop.isRunning())
      loop.quit();
  };

  QObject::connect(&timer, &QTimer::timeout, &loop, [&] {
    ok = false;
    msg = "verify-token timeout";
    if (loop.isRunning())
      loop.quit();
  });

  QObject::connect(apiClient, &ClipboardApiClient::verifyTokenFinished, &loop, onFinished);

  // 发送请求
  apiClient->verifyToken(token);

  // 超时设置 5s
  timer.start(5000);
  loop.exec();

  QObject::disconnect(apiClient, &ClipboardApiClient::verifyTokenFinished, &loop, nullptr);

  return ok;
}

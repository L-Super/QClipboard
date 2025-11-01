#include "Clipboard.h"
#include "SingleApplication"
#include "net/ProtocolHandler.h"
#include "net/ProtocolRegistry.h"
#include "utils/Config.h"
#include "utils/Logger.hpp"
#include "version.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QStyleHints>
#include <QUrl>

void LoadStyleSheet(const QString& stylePath) {
  QFile style(stylePath);
  if (style.open(QFile::ReadOnly)) {
    qApp->setStyleSheet(style.readAll());
  }
  style.close();
}

void ApplyTheme(Qt::ColorScheme scheme) {
  switch (scheme) {
  case Qt::ColorScheme::Dark:
    LoadStyleSheet(":/qss/resources/style_dark.css");
    break;
  case Qt::ColorScheme::Light:
    LoadStyleSheet(":/qss/resources/style.css");
  case Qt::ColorScheme::Unknown:
    LoadStyleSheet(":/qss/resources/style.css");
    break;
  }
  qDebug() << "ApplyTheme to" << scheme;
}

int main(int argc, char* argv[]) {
  SingleApplication a(argc, argv, true);
  auto logFilePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/logs/app.log";
  initLogging(logFilePath.toStdString());

  // 处理命令行参数中的协议URL
  QString protocolUrl;
  if (argc > 1) {
    auto arguments = QString::fromLocal8Bit(argv[1]);
    if (arguments.startsWith("qclipboard://")) {
      protocolUrl = arguments;
    }
  }

  if (a.isSecondary()) {
    qDebug() << "Primary instance PID: " << a.primaryPid();
    qDebug() << "Primary instance user: " << a.primaryUser();
    spdlog::info("Secondary app is launching.");
    if (!protocolUrl.isEmpty()) {
      a.sendMessage(protocolUrl.toUtf8());
    }

    return 0;
  }

#ifdef Q_OS_MACOS
  a.setWindowIcon(QIcon(":/resources/icon-for-mac.png"));
#else
  a.setWindowIcon(QIcon(":/resources/icon.png"));
#endif

  a.setApplicationVersion(VERSION_STR);

  ApplyTheme(QGuiApplication::styleHints()->colorScheme());

  // 控制着当最后一个可视的窗口退出时候，程序是否退出，默认是true
  QApplication::setQuitOnLastWindowClosed(false);

  // 注册自定义协议
  ProtocolRegistry protocolRegistry;
  if (!protocolRegistry.IsProtocolRegistered())
    protocolRegistry.RegisterProtocol();

  auto configFilePath =
      QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/clipboard_settings.json";
  if (auto re = Config::instance().load(configFilePath.toStdString()); !re.has_value()) {
    spdlog::warn("Config file is unexpected. {}", re.error());
  }

  Clipboard c;
  c.show();
  // 创建协议处理器
  ProtocolHandler protocolHandler;
  QObject::connect(&a, &SingleApplication::receivedMessage, &protocolHandler,
                   [&protocolHandler](int instanceId, QByteArray message) {
                     qDebug() << "instance id:" << instanceId << "message:" << message;
                     protocolHandler.HandleProtocolUrl(message);
                   });
  QObject::connect(&a, &SingleApplication::instanceStarted, &c, &Clipboard::show);
  // 连接系统主题变化信号 Qt 6.5 support
  QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, [](Qt::ColorScheme scheme) {
    qDebug() << "System theme change to" << scheme;
    ApplyTheme(scheme);
  });

  // 连接协议处理器的信号到剪贴板对象
  QObject::connect(&protocolHandler, &ProtocolHandler::loginDataReceived, &c,
                   [&c](const UserInfo& userInfo, const QVariantMap& additionalData) {
                     spdlog::info("Data received from custom protocol");
                     qDebug() << "Additional data:" << additionalData;

                     Config::instance().setUserInfo(userInfo);
                     (void)Config::instance().save();

                     c.ReloadSyncServer();

                     // 显示主窗口
                     c.show();
                     c.raise();
                     c.activateWindow();
                   });

  QObject::connect(&protocolHandler, &ProtocolHandler::errorOccurred,
                   [](const QString& errorMessage) { spdlog::info("Protocol error:{}", errorMessage); });

  if (!protocolUrl.isEmpty()) {
    protocolHandler.HandleProtocolUrl(protocolUrl.toUtf8());
  }

  return QApplication::exec();
}

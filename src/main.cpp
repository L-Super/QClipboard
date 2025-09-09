#include "Clipboard.h"
#include "SingleApplication"
#include "utils/Config.h"
#include "utils/ProtocolHandler.h"
#include "utils/ProtocolRegistry.h"
#include "version.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QUrl>

int main(int argc, char *argv[]) {
  SingleApplication a(argc, argv, true);

  // 处理命令行参数中的协议URL
  QString protocolUrl;
  if (argc > 1) {
    auto arguments = QString::fromLocal8Bit(argv[1]);
    if (arguments.startsWith("qclipboard://")) {
      protocolUrl = arguments;
      qDebug() << "Received protocol URL from command line:" << protocolUrl;
    }
  }

  if (a.isSecondary()) {
    qDebug() << "App already running.";
    qDebug() << "Primary instance PID: " << a.primaryPid();
    qDebug() << "Primary instance user: " << a.primaryUser();
    if (!protocolUrl.isEmpty()) {
      a.sendMessage(protocolUrl.toUtf8());
    }

    return 0;
  }

  a.setWindowIcon(QIcon(":/resources/icon.png"));
  a.setApplicationVersion(VERSION_STR);

  QFile style(":/qss/resources/style.css");
  if (style.open(QFile::ReadOnly)) {
    qApp->setStyleSheet(style.readAll());
    style.close();
  }

  // 控制着当最后一个可视的窗口退出时候，程序是否退出，默认是true
  QApplication::setQuitOnLastWindowClosed(false);

  // 注册自定义协议
  ProtocolRegistry protocolRegistry;
  if (!protocolRegistry.IsProtocolRegistered())
    protocolRegistry.RegisterProtocol();

  auto configFilePath =
      QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/clipboard_settings.json";
  Config::instance().load(configFilePath.toStdString());

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
  // 连接协议处理器的信号到剪贴板对象
  QObject::connect(&protocolHandler, &ProtocolHandler::loginDataReceived, &c,
                   [&c](const UserInfo &userInfo, const QVariantMap &additionalData) {
                     qDebug() << "Data received from custom protocol";
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
                   [](const QString &errorMessage) { qDebug() << "Protocol error:" << errorMessage; });

  if (!protocolUrl.isEmpty()) {
    protocolHandler.HandleProtocolUrl(protocolUrl.toUtf8());
  }

  return QApplication::exec();
}

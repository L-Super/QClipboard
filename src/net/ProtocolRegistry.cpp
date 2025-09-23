//
// Created by LMR on 25-9-07.
//

#include "ProtocolRegistry.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>

#include "../utils/Logger.hpp"

ProtocolRegistry::ProtocolRegistry() {}

void ProtocolRegistry::RegisterProtocol(const QString& protocolName) {
#if defined(Q_OS_WIN)
  RegisterWinProtocol(protocolName);
#elif defined(Q_OS_LINUX)
  RegisterLinuxProtocol(protocolName);
#endif
}

void ProtocolRegistry::UnregisterProtocol(const QString& protocolName) {
#if defined(Q_OS_WIN)
  UnregisterWinProtocol(protocolName);
#elif defined(Q_OS_LINUX)
  UnregisterLinuxProtocol(protocolName);
#endif
}

bool ProtocolRegistry::IsProtocolRegistered(const QString& protocolName) {
#if defined(Q_OS_WIN)
  return IsWinProtocolRegistered(protocolName);
#elif defined(Q_OS_LINUX)
  return IsLinuxProtocolRegistered(protocolName);
#endif
  return false;
}

QString ProtocolRegistry::GetProtocolUrl(const QString& protocolName) { return QString("%1://").arg(protocolName); }

#if defined(Q_OS_WIN)
void ProtocolRegistry::RegisterWinProtocol(const QString& protocolName) {
  QSettings settings("HKEY_CURRENT_USER\\Software\\Classes", QSettings::Format::NativeFormat);

  // 注册协议
  QString protocolKey = QString("%1").arg(protocolName);
  QString commandKey = QString("%1/shell/open/command").arg(protocolName);

  // 设置协议描述
  settings.setValue(QString("%1/.").arg(protocolKey), "URL:QClipboard Protocol");
  settings.setValue(QString("%1/URL Protocol").arg(protocolKey), "");

  // 设置协议图标
  settings.setValue(QString("%1/DefaultIcon/.").arg(protocolKey),
                    QString("\"%1,1\"").arg(QCoreApplication::applicationFilePath()));

  // 设置协议命令
  QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
  QString value = "\"" + appPath + R"(" "%1")";
  settings.setValue(QString("%1/.").arg(commandKey), value);

  spdlog::info("Windows protocol registered: {}", protocolName);
}

void ProtocolRegistry::UnregisterWinProtocol(const QString& protocolName) {
  QSettings settings("HKEY_CURRENT_USER\\Software\\Classes", QSettings::Format::NativeFormat);

  // 删除协议注册
  settings.remove(protocolName);

  spdlog::info("Windows protocol unregistered: {}", protocolName);
}

bool ProtocolRegistry::IsWinProtocolRegistered(const QString& protocolName) {
  QSettings settings("HKEY_CURRENT_USER\\Software\\Classes", QSettings::Format::NativeFormat);

  QString protocolKey = QString("%1/.").arg(protocolName);
  return settings.contains(protocolKey);
}

#elif defined(Q_OS_LINUX)
void ProtocolRegistry::RegisterLinuxProtocol(const QString& protocolName) {
  // Linux下通过.desktop文件注册协议
  QString desktopContent = QString(R"([Desktop Entry]
Name=QClipboard Protocol Handler
Comment=Handle QClipboard protocol URLs
Exec=%1 %u
Icon=%2
Type=Application
MimeType=x-scheme-handler/%3;
NoDisplay=true
)")
                               .arg(QCoreApplication::applicationFilePath())
                               .arg(QCoreApplication::applicationFilePath())
                               .arg(protocolName);

  // 写入.desktop文件
  QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) +
                        QString("/%1-protocol-handler.desktop").arg(protocolName);

  QFile desktopFile(desktopPath);
  if (desktopFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&desktopFile);
    out << desktopContent;
    desktopFile.close();

    // 设置可执行权限
    QFile::setPermissions(desktopPath, QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner);

    qDebug() << "Linux protocol registered:" << protocolName;
  }
}

void ProtocolRegistry::UnregisterLinuxProtocol(const QString& protocolName) {
  QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) +
                        QString("/%1-protocol-handler.desktop").arg(protocolName);

  QFile::remove(desktopPath);

  qDebug() << "Linux protocol unregistered:" << protocolName;
}

bool ProtocolRegistry::IsLinuxProtocolRegistered(const QString& protocolName) {
  QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) +
                        QString("/%1-protocol-handler.desktop").arg(protocolName);

  return QFile::exists(desktopPath);
}
#endif

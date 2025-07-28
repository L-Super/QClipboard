//
// Created by LMR on 25-7-27.
//

#include "AutoStartup.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>

AutoStartup::AutoStartup() {}

void AutoStartup::SetAutoStartup(bool enable) {
#if defined(Q_OS_WIN)
  SetWinAutoStartup(enable);
#elif defined(Q_OS_LINUX)
  SetLinuxAutoStartup(enable);
#elif defined(Q_OS_MAC)
  //TODO:
#endif
}

bool AutoStartup::IsAutoStartup() {
#if defined(Q_OS_WIN)
  return IsWinAutoStartup();
#elif defined(Q_OS_LINUX)
  return IsLinuxAutoStartup();
#elif defined(Q_OS_MAC)
  //TODO:
#endif

  // another return true
  return true;
}

#if defined(Q_OS_WIN)
void AutoStartup::SetWinAutoStartup(bool enable) {
  QSettings settings(winRegistry, QSettings::Format::NativeFormat);
  auto appPath = QCoreApplication::applicationFilePath();
  // 程序名称作为注册表中的键,根据键获取对应的值（程序路径）
  QFileInfo fileInfo(appPath);
  // the name as a key
  QString name = fileInfo.baseName();
  if (enable) {
    // second modify
    if (settings.contains(name)) {
      // 如果注册表中的路径和当前程序路径不一样，则表示没有设置自启动或本自启动程序已经更换了路径
      // 获取注册表的路径
      QString oldPath = settings.value(name).toString();
      qDebug() << oldPath;
      // '/' to '\'
      QString newPath = QDir::toNativeSeparators(appPath);
      if (oldPath != newPath)
        settings.setValue(name, newPath);
    } else // first setting
    {
      settings.setValue(name, QDir::toNativeSeparators(appPath));
    }
    qDebug() << "set auto startup finished";
  } else {
    // cancel auto startup
    settings.remove(name);
    qDebug() << "cancel auto startup";
  }
}

bool AutoStartup::IsWinAutoStartup() {
  QSettings settings(winRegistry, QSettings::Format::NativeFormat);
  auto appPath = QCoreApplication::applicationFilePath();
  // 程序名称作为注册表中的键,根据键获取对应的值（程序路径）
  QFileInfo fileInfo(appPath);
  // the name as a key
  QString name = fileInfo.baseName();
  if (settings.contains(name)) {
    return true;
  }
  return false;
}

#elif defined(Q_OS_LINUX)
void AutoStartup::SetLinuxAutoStartup(bool enable) {
  QString text{R"([Desktop Entry]
Categories=Network;Office;
Name=com.QClipboard.QClipboard
Name[zh_CN]=QClipboard
Keywords=clipboard
Keywords[zh_CN]=剪贴板
Type=Application
Exec=/opt/apps/com.QClipboard.QClipboard/files/QClipboard
Icon=logo
X-Deepin-CreatedBy=com.deepin.SessionManager
X-Deepin-AppID=com.QClipboard.QClipboard
Hidden=)"};
  // enable=false -> Hidden=true | enable=true -> Hidden=false
  text.append(enable ? "false\n" : "true\n");

  // need /home/{user}/.config/autostart/com.xxx.desktop path
  auto path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
  QFile file(path + "/autostart/com.QClipboard.QClipboard.desktop");

  try {
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qDebug() << "Autostart file write failed";
      return;
    }
    QTextStream out(&file);

    // write data
    out << text;
  } catch (...) {
    qDebug() << "AutoStart file write fail, raising Exception";
  }

  file.close();
}

bool AutoStartup::IsLinuxAutoStartup() {
  auto path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);

  QFile file(path + "/autostart/com.QClipboard.QClipboard.desktop");
  try {
    if (!file.open(QIODevice::ReadOnly)) {
      qDebug() << "AutoStart file open failed or not exist";
      return false;
    }
  } catch (...) {
    qDebug() << "AutoStart file open failed or not exist, raising Exception";
  }

  QTextStream in(&file);
  QString lastLine;
  QString value;
  while (!in.atEnd()) {
    lastLine = in.readLine();

    auto pos = lastLine.indexOf("Hidden");
    if (pos != -1) {
      // method1:输出右边第N个字符串
      // value = (lastLine.right(lastLine.size() - pos - 7)).trimmed();
      QStringList kv = lastLine.split("=");
      value = kv[1].trimmed();
    }
  }

  file.close();
  return value == "false" ? true : false;
}
#endif

//
// Created by LMR on 25-8-2.
//

#pragma once
#include <QIcon>
#include <QString>

namespace utils {
QString generateDeviceId();

QString macAddress();

QString GetAppName(const QString& appPath);

QIcon GetAppIcon(const QString& appPath);

#ifdef Q_OS_WIN
QString GetClipboardSourceAppPath();

QString GetProcessPath(HWND hwnd);
#endif
} // namespace utils

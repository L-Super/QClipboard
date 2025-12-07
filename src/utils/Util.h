//
// Created by LMR on 25-8-2.
//

#pragma once
#include <QString>

namespace utils {
QString generateDeviceId();

QString macAddress();

QString GetClipboardSourceAppPath();

QString GetAppName(const QString& appPath);
} // namespace utils

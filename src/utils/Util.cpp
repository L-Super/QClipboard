//
// Created by LMR on 25-8-2.
//

#include "Util.h"

#include <QFileIconProvider>
#include <QFileInfo>
#include <QNetworkInterface>
#include <QSysInfo>

#ifdef Q_OS_WIN
// clang-format off
#include <windows.h>
// clang-format on
#endif

namespace utils {
QString generateDeviceId() {
  QString result = macAddress();
  // on Linux systems, this ID is usually permanent
  result += QSysInfo::machineUniqueId();
  return result;
}

QString macAddress() {
  QString result;
  foreach (const QNetworkInterface& netInterface, QNetworkInterface::allInterfaces()) {
    if (!(netInterface.flags() & QNetworkInterface::IsLoopBack)) {
      result = netInterface.hardwareAddress();
      break;
    }
  }
  return result;
}

#ifdef Q_OS_WIN
QString GetProcessPath(HWND hwnd) {
  DWORD pid = 0;
  GetWindowThreadProcessId(hwnd, &pid);
  if (pid == 0)
    return {};

  // win 10 later only need PROCESS_QUERY_LIMITED_INFORMATION
  HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
  if (!hProc)
    return {};

  wchar_t buf[MAX_PATH] = {};
  DWORD size = MAX_PATH;
  QString result;

  if (QueryFullProcessImageNameW(hProc, 0, buf, &size)) {
    result = QString::fromWCharArray(buf, size);
  }

  CloseHandle(hProc);
  return result;
}

QString GetClipboardSourceAppPath() {
  HWND clipboardOwner = GetClipboardOwner();
  if (!clipboardOwner) {
    return {};
  }

  return GetProcessPath(clipboardOwner);
}
#endif

QString GetAppName(const QString& appPath) {
  QFileInfo info(appPath);
  return info.fileName();
}

QIcon GetAppIcon(const QString& appPath) {
  QFileIconProvider iconProvider;
  return iconProvider.icon(QFileInfo(appPath));
}
} // namespace utils
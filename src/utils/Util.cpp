//
// Created by LMR on 25-8-2.
//

#include "Util.h"

#include <QNetworkInterface>
#include <QSysInfo>
#include <QFileInfo>
#include <QFileIconProvider>

#ifdef Q_OS_WIN
// clang-format off
#include <windows.h>
#include <Psapi.h>

#pragma comment(lib, "psapi.lib")
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

QString GetClipboardSourceAppPath() {
#ifdef Q_OS_WIN
  HWND clipboardOwner = GetClipboardOwner();
  if (!clipboardOwner) {
    return "System or Unknown";
  }

  DWORD processId = 0;
  GetWindowThreadProcessId(clipboardOwner, &processId);

  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
  if (!hProcess) {
    // 尝试用较少权限打开
    hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!hProcess)
      return "Unknown";
  }

  wchar_t filename[MAX_PATH] = {0};
  GetModuleFileNameEx(hProcess, nullptr, filename, MAX_PATH);

  CloseHandle(hProcess);
  return QString::fromWCharArray(filename);
#else
  return {};
#endif
}

QString GetAppName(const QString& appPath) {
  QFileInfo info(appPath);
  return info.fileName();
}

QIcon GetAppIcon(const QString& appPath) {
  QFileIconProvider iconProvider;
  return iconProvider.icon(QFileInfo(appPath));
}
} // namespace utils
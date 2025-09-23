//
// Created by LMR on 25-8-2.
//

#include "Util.h"

#include <QNetworkInterface>
#include <QSysInfo>

namespace utils {
QString generateDeviceId() {
  QString result = macAddress();
  // on Linux systems, this ID is usually permanent
  result += QSysInfo::machineUniqueId();
  return result;
}

QString macAddress() {
  QString result;
  foreach (const QNetworkInterface& interface, QNetworkInterface::allInterfaces()) {
    if (!(interface.flags() & QNetworkInterface::IsLoopBack)) {
      result = interface.hardwareAddress();
      break;
    }
  }
  return result;
}
} // namespace utils
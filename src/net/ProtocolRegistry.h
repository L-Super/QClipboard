//
// Created by LMR on 25-9-07.
//

#pragma once

#include <QObject>
#include <QString>

class ProtocolRegistry : public QObject {
  Q_OBJECT
public:
  ProtocolRegistry();
  ~ProtocolRegistry() override = default;

  // 注册自定义协议
  void RegisterProtocol(const QString& protocolName = "floward");

  // 取消注册自定义协议
  void UnregisterProtocol(const QString& protocolName = "floward");

  // 检查协议是否已注册
  bool IsProtocolRegistered(const QString& protocolName = "floward");

  // 获取协议URL
  QString GetProtocolUrl(const QString& protocolName = "floward");

private:
#if defined(Q_OS_WIN)
  void RegisterWinProtocol(const QString& protocolName);
  void UnregisterWinProtocol(const QString& protocolName);
  bool IsWinProtocolRegistered(const QString& protocolName);
#elif defined(Q_OS_LINUX)
  void RegisterLinuxProtocol(const QString& protocolName);
  void UnregisterLinuxProtocol(const QString& protocolName);
  bool IsLinuxProtocolRegistered(const QString& protocolName);
#endif
};

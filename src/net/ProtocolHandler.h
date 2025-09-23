//
// Created by LMR on 25-9-07.
//

#pragma once

#include "ClipboardStruct.h"

#include <QObject>
#include <QString>
#include <QVariantMap>

class ProtocolHandler : public QObject {
  Q_OBJECT

public:
  explicit ProtocolHandler(QObject* parent = nullptr);
  ~ProtocolHandler() override = default;

  // 处理协议URL
  void HandleProtocolUrl(const QString& url);

  // 解析URL参数
  QVariantMap ParseUrlParameters(const QString& url);

signals:
  // 登录数据信号
  void loginDataReceived(UserInfo userInfo, const QVariantMap& additionalData);

  // 错误信号
  void errorOccurred(const QString& errorMessage);

private:
  // 验证URL格式
  bool ValidateUrl(const QString& url);

  // 解码URL参数
  QString DecodeUrlParameter(const QString& parameter);

  // 编码URL参数
  QString EncodeUrlParameter(const QString& parameter);
};

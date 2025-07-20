#pragma once
#include <QString>

enum class DeviceType { macos, windows, linux, ios, android, web };

enum ClipboardDataType { text, image, file };

struct User {
  QString email;
  QString password;
  QString deviceId;
  QString deviceName;
  DeviceType deviceType;
};

struct ClipboardData {
  QString data;
  ClipboardDataType type;
  QString meta;
};

struct Token {
  QString accessToken;
  QString refreshToken;
};
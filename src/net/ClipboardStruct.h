#pragma once
#include "nlohmann/json.hpp"
#include <QString>

enum class DeviceType { macos, windows, linux, ios, android, web };

enum class ClipboardDataType { text, image, file };

struct User {
  QString email;
  QString password;
  QString deviceId;
  QString deviceName;
  DeviceType deviceType;
};

struct ClipboardData {
  QByteArray data;
  ClipboardDataType type;
  QString meta;
};

struct Token {
  QString accessToken;
  QString refreshToken;
};

struct ServerConfig {
  std::string url;
  std::string user;
  std::string password;
  std::string device_id;
  std::string device_name;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServerConfig, url, user, password, device_id, device_name)
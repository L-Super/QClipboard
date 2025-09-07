#pragma once
#include "nlohmann/json.hpp"
#include <QString>

// fix `error: expected identifier before numeric constant`
#if defined(Q_OS_LINUX)
#pragma push_macro("linux")
#undef linux
#endif

enum class DeviceType { macos, windows, linux, ios, android, web };

#if defined(Q_OS_LINUX)
#pragma pop_macro("linux")
#endif

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

struct UserInfo {
  std::string email;
  std::string token;
  std::string device_name;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UserInfo, email, token, device_name)
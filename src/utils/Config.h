//
// Created by LMR on 25-7-22.
//

#pragma once

#include <filesystem>
#include <optional>
#include <expected>

#include "nlohmann/json.hpp"

#include "../net/ClipboardStruct.h"

class Config {
private:
  Config() = default;
  ~Config();

public:
  static Config &instance() {
    static Config config;
    return config;
  }

  std::expected<bool, std::string> load(const std::filesystem::path &file);
  bool save() const;

  template <typename T> void set(const std::string &key, T &&value) {
    // set data to json first
    data_[key] = std::forward<T>(value);
  }

  template <typename T> std::optional<T> get(const std::string &key) const {
    if (!data_.contains(key))
      return {};
    return data_.at(key).get<T>();
  }

  void setServerConfig(const ServerConfig& server);
  std::optional<ServerConfig> getServerConfig() const;

  void setUserInfo(const UserInfo& userInfo);
  std::optional<UserInfo> getUserInfo() const;

public:
  static std::string defaultApiUrl;

protected:
  void fillDefaultValues();

private:
  nlohmann::json data_;
  std::filesystem::path filepath_{};
};
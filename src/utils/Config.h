//
// Created by LMR on 25-7-22.
//

#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>
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

  bool load(const std::filesystem::path &file);
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

private:
  nlohmann::json data_;
  std::filesystem::path filepath_;
};
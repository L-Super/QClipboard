//
// Created by LMR on 25-7-22.
//

#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <optional>

class Config {

public:
  Config() = default;
  ~Config();
  explicit Config(const std::filesystem::path &file);

  bool load(const std::filesystem::path &file);
  bool save() const;

  template <typename T> void set(const std::string &key, T &&value) {
    data_[key] = std::forward<T>(value);
  }

  template <typename T> std::optional<T> get(const std::string &key) const {
    if (!data_.contains(key))
      return {};
    return data_.at(key).get<T>();
  }

private:
  nlohmann::json data_;
  std::filesystem::path filepath_;
};
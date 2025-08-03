//
// Created by LMR on 25-7-22.
//

#include "Config.h"
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

Config::~Config() { save(); }

bool Config::load(const fs::path &file) {
  if (!fs::exists(file)) {
    auto parentPath = file.parent_path();
    fs::create_directories(parentPath);
    // create an empty file
    std::ofstream out(file);
  }
  filepath_ = file;
  std::ifstream in(file);
  if (!in.is_open())
    return false;

  // Check if file is empty
  in.seekg(0, std::ios::end);
  if (in.tellg() == 0) {
    // File is empty, do not parse
    return false;
  }
  in.seekg(0, std::ios::beg);

  try {
    data_ = nlohmann::json::parse(in);
  } catch (const std::exception &e) {

    return false;
  }

  return true;
}

bool Config::save() const {
  std::ofstream out(filepath_);
  if (!out)
    return false;
  if (!data_.empty())
    out << data_.dump(4);
  return true;
}

void Config::setServerConfig(const ServerConfig& server) {
  data_["server"] = server;
}

std::optional<ServerConfig> Config::getServerConfig() const {
    if (!data_.contains("server"))
        return std::nullopt;

    try {
      ServerConfig server = data_["server"];
      return std::make_optional(server);
    } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
    }


    return std::nullopt;
}
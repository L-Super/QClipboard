//
// Created by LMR on 25-7-22.
//

#include "Config.h"
#include <fstream>

namespace fs = std::filesystem;

Config::~Config() {
    save();
}

bool Config::load(const fs::path& file) {
    filepath_ = file;
    std::ifstream in(file);
    if (!in) return false;
    data_ = nlohmann::json::parse(in, nullptr, false);
    return !data_.is_discarded();
}

Config::Config(const fs::path& file) {
    load(file);
}

bool Config::save() const {
    std::ofstream out(filepath_);
    if (!out) return false;
    out << data_.dump(4);
    return true;
}
#pragma once
#include "log.hpp"
#include "robust_str.hpp"

#include <filesystem>
#include <vector>
#include <iostream>
#include <functional>

namespace fs = std::filesystem;

/*****************************
 *  文件操作
 *****************************/
namespace FileAction {
    bool createSubDir(std::string subdir, bool debug = false);

    bool recursiveCreateDir(std::string recur_dir, std::string pagin_str = "/");

    std::vector<fs::path> getFiles(std::string dir,
        std::function<bool(fs::path)> checker = [](fs::path p) {
            return true;
        });
    std::vector<fs::path> getFiles(std::string dir, std::string postfix);

}

/**
 * @file config.hpp
 * @author Joe1sn (joe1sn23333@gmail.com)
 * @brief 配置文件相关功能
 * @version 0.1
 * @date 2025-02-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "constant.hpp"
#include "log.hpp"
#include "robust_str.hpp"


#include <iostream>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <vector>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace config {

    //配置文件类
    class configPrototype {
    public:
        configPrototype(std::string confPath = Constant::configPath);
        ~configPrototype() {};

        void printConfig();


        std::string title;
        std::string subtitle;
        std::string author;
        std::string webPath;

        std::string source_dir;
        std::string public_dir;
        std::string tag_dir;
        std::string archive_dir;
        std::string category_dir;

        size_t per_page;
        std::string pagination_dir;

        std::string repo;
        std::string branch;

        fs::path postDir;
        fs::path webRootDir;

    private:

        void parserConfig();
        bool checkConfig();

        std::string rawData;    //原始配置文件数据
        json jsonConfig;        //json化后的配置文件

    };

    inline config::configPrototype config;

}
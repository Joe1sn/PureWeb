/**
 * @file constant.hpp
 * @author Joe1sn (joe1sn23333@gmail.com)
 * @brief 所有常量
 * @version 0.1
 * @date 2025-02-20
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include <iostream>
namespace Constant {
    inline constexpr const char* configPath = "./config.json";
    inline constexpr const char* defaultConfig = "{\"Site\": {\"title\": \"PureWeb Blog\", \"subtitle\": \"\", \"author\": \"Joe1sn\"}, \"Directory\": {\"source_dir\": \"source\", \"public_dir\": \"public\", \"tag_dir\": \"tags\", \"archive_dir\": \"archives\", \"category_dir\": \"categories\"}, \"Pagination\": {\"per_page\": 20, \"pagination_dir\": \"page\"}, \"Deploy\": {\"repo\": \"https://github.com/Joe1sn/TestRepo\", \"branch\": \"main\"}}";
    inline constexpr const char* defaultWebDir = "./website";
    inline constexpr const char* defaultPostDir = "./posts";
    inline constexpr const char* defaultThemeDir = "./theme";

    inline constexpr const char* defaultThemeName = "github";
    inline constexpr const char* timeFormat = "%Y-%m-%d %H:%M:%S";
}

namespace HTML {
    inline constexpr const char* header = "h";
    inline constexpr const char* newLine = "p";
    inline constexpr const char* code = "code";
    inline constexpr const char* list = "li";
}

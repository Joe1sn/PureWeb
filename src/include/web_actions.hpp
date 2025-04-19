/**
 * @file actions.hpp
 * @author Joe1sn (joe1sn23333@gmail.com)
 * @brief 所有功能
 * @version 0.1
 * @date 2025-02-20
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once
#include "log.hpp"
#include "constant.hpp"
#include "config.hpp"
#include "markdown.hpp"
#include "gitcmd.hpp"
#include "robust_str.hpp"
#include "file_actions.hpp"
#include "html.hpp"


#include <filesystem>
#include <vector>
#include <iostream>
#include <functional>
#include <map>

namespace action {


    /*****************************
     *  网站结构
     *****************************/
    class website
    {
    public:
        fs::path rootDir;
        fs::path postDir;
        fs::path sourceDir;
        fs::path publicDir;
        fs::path tagsDir;
        fs::path archiveDir;
        fs::path categoryDir;
        fs::path mainPageDir;

        // python example:  {"tagA":[<markdownFileA>,<markdownFileB>,...]}
        std::map<std::string, std::vector<std::unique_ptr<markdown::markdown>>> tagAndMd;
        std::map<std::string, std::vector<std::unique_ptr<markdown::markdown>>> categloriesAndMd;

        std::map<std::string, fs::path> mdToFilePath;   //md的文件名唯一，所以 文件名:Html文件路径

        std::vector<std::string> cssFiles;

    private:
        std::vector<std::unique_ptr<markdown::markdown>> allMdFiles;
        std::vector<fs::path> srcMdPath;

        std::string paginHtml(size_t min, size_t currentIndex, size_t max, std::string currentPath, std::string firstPagePath = "");

        //保存目录
        void saveHtml();

        void saveArchive();

        void saveCategory();
        void saveTags();



        //生成各种index
        void genIndexHtml();

    public:
        website();
        ~website() {};
        void initWebsite(); //初始化网站
        std::vector<std::unique_ptr<markdown::markdown>> getAllMarkdownFiles();

        bool renderMarkdowns(); //所有markdown转html并到相应文件夹
        bool renderIndexPage(); //渲染所有路径下的index文件

    };


}
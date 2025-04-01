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
#include "html.hpp"


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

    private:
        std::vector<std::unique_ptr<markdown::markdown>> allMdFiles;
        std::vector<fs::path> srcMdPath;

        //保存目录
        std::vector<fs::path> saveArchive(
            std::function<bool(std::unique_ptr<markdown::markdown>&, fs::path)>function = \
            [](std::unique_ptr<markdown::markdown>& file, fs::path srcPath) {
                return true;
            });

        fs::path saveCategory(std::unique_ptr<markdown::markdown>& file, fs::path srcPath);
        fs::path saveTags(std::unique_ptr<markdown::markdown>& file, fs::path srcPath);

        bool saveCateAndTag(std::unique_ptr<markdown::markdown>& file, fs::path srcPath);

        //生成各种index
        void genIndexHtml();
        // bool genArchiveIndex();
        // bool genCategoryIndex();
        // bool genTagsIndex();

    public:
        website();
        ~website() {};
        void initWebsite(); //初始化网站
        std::vector<std::unique_ptr<markdown::markdown>> getAllMarkdownFiles();

        bool renderMarkdowns(); //所有markdown转html并到相应文件夹
        bool renderIndexPage(); //渲染所有路径下的index文件

    };


}
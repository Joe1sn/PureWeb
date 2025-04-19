#include "include/web_actions.hpp"

namespace action {

    /*****************************
     *  网站结构
     *****************************/
    website::website() {
        this->rootDir = config::config.webRootDir;
        this->postDir = config::config.postDir;

        this->sourceDir = this->rootDir / config::config.source_dir;
        this->publicDir = this->rootDir / config::config.public_dir;
        this->tagsDir = this->rootDir / config::config.tag_dir;
        this->archiveDir = this->rootDir / config::config.archive_dir;
        this->categoryDir = this->rootDir / config::config.category_dir;
        this->mainPageDir = this->rootDir / config::config.pagination_dir;

        std::vector<std::unique_ptr<markdown::markdown>> result = {};
        if (!fs::exists(this->postDir)) {
            logger::error << "no such dir: " << fs::absolute(this->postDir) << "\n";
        }
        else {
            this->srcMdPath = FileAction::getFiles(fs::absolute(this->postDir).string(),
                [&result](fs::path p) {
                    if (p.extension().string() == ".md") {
                        result.push_back(std::move(std::make_unique<markdown::markdown>(p.string())));
                        return true;
                    }
                    return false;
                });
        }
        this->allMdFiles = std::move(result);

    }

    //初始化网站
    void website::initWebsite() {

        if (!FileAction::createSubDir(Constant::defaultWebDir)) {
            logger::error << "Failed Init: create website directory failed\n";
            return;
        }
        else
            logger::success << "created website root directory\n";

        if (!FileAction::createSubDir(Constant::defaultPostDir)) {
            logger::error << "Failed Init: create pre_post directory failed\n";
            return;
        }
        else
            logger::success << "created post root directory\n";

        if (!FileAction::createSubDir(Constant::defaultThemeDir)) {
            logger::error << "Failed Init: create theme directory failed\n";
            return;
        }
        else
            logger::success << "created theme root directory\n";

        fs::path rootdir = Constant::defaultWebDir;

        std::vector<std::string> paths = {
            config::config.source_dir,
            config::config.public_dir,
            config::config.tag_dir,
            config::config.archive_dir,
            config::config.category_dir,
            config::config.pagination_dir
        };

        for (const std::string path : paths) {
            fs::path tempdir = path;
            fs::path subdir = Constant::defaultWebDir / tempdir;
            if (!FileAction::createSubDir(subdir.string()))
                return;
        }

        gitcmd::initRepo();
    }

    //获得所有md文件
    std::vector<std::unique_ptr<markdown::markdown>> \
        website::getAllMarkdownFiles() {
        std::vector<std::unique_ptr<markdown::markdown>> result;
        if (!fs::exists(this->postDir)) {
            logger::error << "no such dir: " << fs::absolute(this->postDir) << "\n";
            return result;
        }
        std::vector<fs::path> files = FileAction::getFiles(fs::absolute(this->postDir).string(),
            [&result](fs::path p) {
                if (p.extension().string() == ".md") {
                    result.push_back(std::move(std::make_unique<markdown::markdown>(p.string())));
                    return true;
                }
                return false;
            });
        return result;
    }


    //合成翻页
    std::string website::paginHtml(size_t min, size_t currentIndex, size_t max, std::string currentPath, std::string firstPagePath) {
        std::string result = "";
        //0. 检查
        if (!(min <= currentIndex && currentIndex <= max)) {
            logger::error << "??? wrong index arrange\n";
            return result;
        }
        //1. 为第一页
        if (min == currentIndex) {
            if (currentIndex == max) {  //只有一页，没有下标
                result = "";
            }
            else {
                if (!currentPath.ends_with("/"))
                    result = "<div class=\"pagination\"><a class=\"pagination__link pagination__next\" href=\"" + currentPath + std::to_string(currentIndex + 1) + "/index.html\">next</a></div>";
                else
                    result = "<div class=\"pagination\"><a class=\"pagination__link pagination__next\" href=\"" + currentPath.substr(0, currentPath.length() - 1) + std::to_string(currentIndex + 1) + "/index.html\">next</a></div>";
            }
        }

        //2.为min+1页
        else if (min + 1 == currentIndex) {
            if (currentIndex == max) {//共两页，这个为最后一页
                result = "<div class=\"pagination\"><a class=\"pagination__link pagination__prev\" href=\"" + firstPagePath + "\">prev</a></div>";
            }
            else {
                if (!currentPath.ends_with("/"))
                    result = "<div class=\"pagination\"><a class=\"pagination__link pagination__prev\" href=\"" + firstPagePath + "\">prev</a><a class=\"pagination__link pagination__next\" href=\"" + currentPath + std::to_string(currentIndex + 1) + "/index.html\">next</a></div>";
                else
                    result = "<div class=\"pagination\"><a class=\"pagination__link pagination__prev\" href=\"" + firstPagePath + "\">prev</a><a class=\"pagination__link pagination__next\" href=\"" + currentPath.substr(0, currentPath.length() - 1) + std::to_string(currentIndex + 1) + "/index.html\">next</a></div>";
            }
        }
        //3.为中间页
        else if (min < currentIndex && currentIndex < max) {
            if (!currentPath.ends_with("/"))
                result = "<div class=\"pagination\"><a class=\"pagination__link pagination__prev\" href=\"" + currentPath + std::to_string(currentIndex - 1) + "\">prev</a><a class=\"pagination__link pagination__next\" href=\"" + currentPath + std::to_string(currentIndex + 1) + "/index.html\">next</a></div>";
            else
                result = "<div class=\"pagination\"><a class=\"pagination__link pagination__prev\" href=\"" + currentPath.substr(0, currentPath.length() - 1) + std::to_string(currentIndex - 1) + "\">prev</a><a class=\"pagination__link pagination__next\" href=\"" + currentPath.substr(0, currentPath.length() - 1) + std::to_string(currentIndex + 1) + "/index.html\">next</a></div>";
        }
        //4.真·最后一页
        else if (currentIndex == max) {
            if (!currentPath.ends_with("/"))
                result = "<div class=\"pagination\"><a class=\"pagination__link pagination__prev\" href=\"" + currentPath + std::to_string(currentIndex - 1) + "\">prev</a></div>";
            else
                result = "<div class=\"pagination\"><a class=\"pagination__link pagination__prev\" href=\"" + currentPath.substr(0, currentPath.length() - 1) + std::to_string(currentIndex - 1) + "\">prev</a></div>";
        }
        return result;
    }

    //保存所有Html文件
    void website::saveHtml() {
        fs::path tempPath;
        std::ofstream fileObj;
        std::string tempStr;
        std::string htmlFilename;

        for (auto& file : this->allMdFiles) {
            htmlFilename = file->path.stem().string() + ".html";
            file->parser();
            tempStr = file->toHtml(this->cssFiles);

            tempPath = config::config.webRootDir / rstr::formatTimestamp(file->date);
            tempPath = tempPath / htmlFilename;

            if (!fs::exists(tempPath)) {
                FileAction::recursiveCreateDir(tempPath.parent_path().string());
            }
            fileObj = std::ofstream(tempPath);
            fileObj.write(tempStr.c_str(), tempStr.length());
            fileObj.close();
            this->mdToFilePath[file->path.filename().string()] = tempPath;
            logger::success << "translate successfully file to: " << file->path.filename().string() << "\n";
        }
    }

    //保存到archive目录
    void website::saveArchive() {

        size_t pageIndex = 0;
        size_t counter = 0;
        auto page = Html::html(config::config.title, config::config.subtitle, "", this->cssFiles);
        std::string bodyContent = "";
        const std::string sectionHeader = "<section class=\"archive\">";
        const std::string sectionEnd = "</section>";
        std::string lastYear = "";
        std::string currentYear = "";

        std::sort(this->allMdFiles.begin(), this->allMdFiles.end(),
            [](std::unique_ptr<markdown::markdown>& a, std::unique_ptr<markdown::markdown>& b) {
                return a->date > b->date;
            }
        );

        for (auto& file : this->allMdFiles) {
            currentYear = file->dateStr.substr(0, 4);
            if (lastYear == "") {
                lastYear = currentYear;
                bodyContent += "<h3 class=\"archive__title article__title\">" + lastYear + "</h3>";
            }
            else if (currentYear != lastYear) {
                bodyContent += "<h3 class=\"archive__title article__title\">" + currentYear + "</h3>";
                lastYear = currentYear;
            }
            if (currentYear == lastYear) {
                bodyContent += "<ul class=\"archive__list\"><li class=\"archive__list__item\"><span class=\"archive__list__item__text metadata\">";
                bodyContent += file->dateStr;
                bodyContent += "</span><a class=\"archive__list__item__link\" href=\"" + config::config.webPath + fs::relative(this->mdToFilePath[file->path.filename().string()], Constant::defaultWebDir).generic_string() + "\">";
                bodyContent += file->articleTile;
                bodyContent += "</a></li></ul>";
            }
            ++counter;
            if (counter % config::config.per_page == 0) {
                pageIndex = size_t(counter / config::config.per_page);

                //1.创建文件夹 /website/archive/pagex
                fs::path pagePath = "";
                if (pageIndex != 1) {
                    pagePath = this->archiveDir / ("page" + std::to_string(pageIndex));
                    if (!FileAction::recursiveCreateDir(pagePath.string())) {
                        logger::error << "can't create dir: " << pagePath.string() << "\n";
                        return;
                    }
                }
                else {
                    pagePath = this->archiveDir;
                }
                pagePath /= "index.html";

                //2.合成翻页
                bodyContent += this->paginHtml(1, pageIndex, size_t((this->allMdFiles.size()) / config::config.per_page),
                    "/" + fs::relative(fs::path(this->archiveDir / config::config.pagination_dir), config::config.webRootDir).generic_string(), "/" + fs::relative(this->archiveDir, config::config.webRootDir).generic_string());

                //3.在文件夹下创建index.html
                // logger::debug << "/" + fs::relative(pagePath.parent_path(), config::config.webRootDir).generic_string() << "\n";
                auto fileObj = std::ofstream(pagePath);
                page.bodySetter(bodyContent);
                std::string tempStr = page.to_string();
                fileObj.write(tempStr.c_str(), tempStr.length());
                fileObj.close();

                //5.保存
                bodyContent = "";
                page.bodySetter(bodyContent);
            }
        }
    }

    //生成主页html
    void website::genIndexHtml() {
        std::string filename = "index.html";
        std::string mdFormat = "";
        std::string htmlFormat = "";

        //生成主页
        auto indexPage = Html::html(config::config.title, config::config.subtitle, "", this->cssFiles);

        //按照时间排序，约新越靠前
        std::sort(this->allMdFiles.begin(), this->allMdFiles.end(),
            [](std::unique_ptr<markdown::markdown>& a, std::unique_ptr<markdown::markdown>& b) {
                return a->date > b->date;
            }
        );
        size_t counter = 0;
        // size_t totalPage = size_t((this->allMdFiles.size() + config::config.per_page - 1) / config::config.per_page);
        size_t totalPage = size_t((this->allMdFiles.size()) / config::config.per_page);
        size_t currentIndex = 0;
        fs::path archivePath = this->mainPageDir;
        FileAction::recursiveCreateDir(fs::relative(archivePath).string(), "\\");

        for (auto& mdFile : this->allMdFiles) {
            //开始生成每个page的html
            //1.生成markdown原始排版
            fs::path p = mdFile->path;
            std::string title = mdFile->articleTile;
            if (mdFile->articleTile.ends_with("\n"))
                title = mdFile->articleTile.substr(0, mdFile->articleTile.length() - 1);
            //1.1 合成标题
            title = rstr::replaceAll(title, "[", "\\[");
            title = rstr::replaceAll(title, "]", "\\]");
            title = rstr::replaceAll(title, "(", "\\(");
            title = rstr::replaceAll(title, ")", "\\)");

            mdFormat += "# [" + title + "](" + config::config.webPath + fs::relative(this->mdToFilePath[mdFile->path.filename().string()], config::config.webRootDir).generic_string();
            mdFormat += ")\n\n";
            mdFormat += mdFile->dateStr + "\n\n";

            //1.2 合成简介
            std::vector<std::string> lines;
            std::istringstream stream(mdFile->summary);
            std::string line;
            while (std::getline(stream, line)) {
                lines.push_back(line);
            }

            for (auto line : lines) {
                std::string str(line.begin(), line.end());
                if (line.empty())
                    mdFormat += "> Read More About it\n";
                else
                    mdFormat += "> " + str + "\n";
            }
            mdFormat += "\n\n";

            counter++;
            if (counter % config::config.per_page == 0) {
                ++currentIndex;
                htmlFormat += cmark_markdown_to_html(mdFormat.c_str(), mdFormat.length(), CMARK_OPT_UNSAFE);
                //1.创建文件夹 /website/page/x/
                fs::path pagePath = "";
                if (currentIndex != 1) {
                    pagePath = this->mainPageDir / std::to_string(currentIndex);
                    if (!FileAction::recursiveCreateDir(pagePath.string())) {
                        logger::error << "can't create dir: " << pagePath.string() << "\n";
                        return;
                    }
                }
                else {
                    pagePath = config::config.webRootDir;
                }
                pagePath /= "index.html";

                // 2.合成翻页
                htmlFormat += this->paginHtml(1, currentIndex, totalPage, \
                    "/" + config::config.pagination_dir + "//",
                    "/"
                );

                indexPage.bodySetter(htmlFormat);
                auto fileObj = std::ofstream(pagePath);
                std::string tempStr = indexPage.to_string();
                fileObj.write(tempStr.c_str(), tempStr.length());
                fileObj.close();

                //5.保存
                htmlFormat = "";
                mdFormat = "";
                indexPage.bodySetter(htmlFormat);

            }
        }
        logger::success << "[Archive Index]create successful\n";

    }

    //对所有md文件转到html
    bool website::renderMarkdowns() {
        // 0. cssFile检查和复制
        std::vector<fs::path>cssPath = FileAction::getFiles(Constant::defaultThemeDir, ".css");
        for (auto file : cssPath)
        {
            try
            {
                fs::copy(file, fs::path(Constant::defaultWebDir));
            }
            catch (...) {}
            this->cssFiles.push_back(file.filename().string());
        }

        // 1. 保存所有md到html文件，并记录对应关系
        this->saveHtml();
        // 2. 生成主页
        this->genIndexHtml();
        // 3. archive文件夹根据时间和大小分配Page
        this->saveArchive();
        // 4. cate 生成Page
        // 5. Tag 生成Page


        /*
        fs::path srcPath = config::config.postDir;
        fs::path dstPath = config::config.webRootDir;

        if (!fs::exists(srcPath) || !fs::exists(dstPath)) {
            logger::error << "can't find source file path: " << fs::absolute(srcPath)
                << "or website path: " << fs::absolute(dstPath) << "\n";
        }
        this->saveArchive([this](std::unique_ptr<markdown::markdown>& file, fs::path srcPath) {
            return this->saveCateAndTag(file, srcPath);
            });

        this->genIndexHtml();
        */
        return true;
    }

    //渲染所有路径下的index文件
    bool website::renderIndexPage() {
        return true;
    }

}
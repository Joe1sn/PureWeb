#include "include/actions.hpp"
namespace FileAction {
    /*****************************
    *  文件操作类
    *****************************/
    //创建子文件夹
    bool createSubDir(std::string subdir, bool debug) {
        fs::path dir = subdir;
        if (!fs::exists(dir)) { //该文件夹不存在
            if (fs::create_directory(fs::absolute(dir))) {
                if (debug)
                    logger::success << "Directory:" << fs::absolute(dir) << " created successfully\n";
            }

            else {
                logger::error << "Failed to create directory: " << fs::absolute(dir) << "\n";
                return false;
            }
            return true;
        }
        else {
            // logger::warn << "Directory:" << fs::absolute(dir) << " already exists.\n";
            return true;
        }
    }

    //递归式创建文件夹
    bool recursiveCreateDir(std::string recur_dir, std::string pagin_str) {
        fs::path path = "";
        for (const auto str : Html::splitStr(recur_dir, pagin_str))
        {
            path = path / str;
            if (!createSubDir(path.string())) { //及时退出避免死循环
                logger::error << "can't create sub directoty: " << str << "\n";
                break;
            }
        }
        return true;
    }

    //遍历目录下所有文件
    //checker为路径判断的方式
    std::vector<fs::path> getFiles(std::string dir, std::function<bool(fs::path)>checker) {
        std::vector<fs::path> result = {};
        if (!fs::exists(dir)) {
            logger::error << "No such directory: " << dir << "\n";
            return result;
        }
        fs::path root = dir;
        // 遍历目录
        for (const auto& entry : fs::recursive_directory_iterator(root)) {
            if (fs::is_regular_file(entry)) { // 检查是否为普通文件
                if (checker(entry))             //使用checker检查
                    result.push_back(entry);
            }
        }
        return result;
    }

    //根据后缀名选择文件夹下的文件
    std::vector<fs::path> getFiles(std::string dir, std::string postfix) {
        std::vector<fs::path> result = {};
        result = FileAction::getFiles(dir,
            [postfix](fs::path p) {
                if (p.extension().string() == postfix)
                    return true;
                return false;
            });
        return result;
    }

}

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
            config::config.category_dir
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

    //保存到archive目录
    std::vector<fs::path> website::saveArchive(
        std::function<bool(std::unique_ptr<markdown::markdown>&, fs::path)>function) {

        std::vector<fs::path> result = {};
        size_t pageIndex = 0;
        fs::path tempPath;
        std::ofstream fileObj;
        std::string tempStr;
        std::string filename;

        for (auto& file : this->allMdFiles)
        {
            filename = file->path.stem().string() + ".html";
            file->parser();
            tempStr = file->toHtml();

            //TODO: archive by archive/{year}/{mouth}/{day}
            tempPath = this->archiveDir / Html::formatTimestamp(file->date);
            tempPath = tempPath / filename;

            if (!fs::exists(tempPath)) {
                FileAction::recursiveCreateDir(tempPath.parent_path().string());
            }
            fileObj = std::ofstream(tempPath);
            fileObj.write(tempStr.c_str(), tempStr.length());
            fileObj.close();
            result.push_back(tempPath);
            function(file, tempPath);
            logger::success << "[Archive]translate successfully with file: " << fs::absolute(file->path) << "\n";
            pageIndex++;
        }

        return result;

    }

    //保存文件到目录
    fs::path website::saveCategory(std::unique_ptr<markdown::markdown>& file, fs::path srcPath) {
        std::string filename = file->path.stem().string() + ".html";
        fs::path dstPath = this->categoryDir / file->categories / filename;
        try
        {
            if (!fs::exists(dstPath.parent_path()))
                FileAction::createSubDir(fs::absolute(dstPath.parent_path()).string());
            fs::copy_file(srcPath, dstPath);
        }
        catch (...)
        {
            // std::cerr << e.what() << '\n';
        }

        return dstPath;
    }

    //根据tags复制文件
    fs::path website::saveTags(std::unique_ptr<markdown::markdown>& file, fs::path srcPath) {
        std::string filename = file->path.stem().string() + ".html";
        fs::path tempPath = "";
        try
        {
            for (auto tagPath : file->tags)
            {
                tempPath = this->tagsDir / tagPath / filename;
                if (!fs::exists(tempPath)) {
                    FileAction::createSubDir(tempPath.parent_path().string());
                }
                fs::copy_file(srcPath, tempPath);
            }
        }
        catch (...)
        {
            // std::cerr << e.what() << '\n';
        }

        return tempPath;
    }

    //回调callback，在saveArchive中保存文件和tags
    bool website::saveCateAndTag(std::unique_ptr<markdown::markdown>& file, fs::path srcPath) {
        fs::path result = this->saveTags(file, srcPath);
        result = this->saveCategory(file, srcPath);
        return true;
    }

    //生成主页html
    void website::genIndexHtml() {
        std::string filename = "index.html";
        std::string mdFormat = "";

        //生成主页
        std::string indexPage = "";
        std::string standardStart = "<!DOCTYPE html>\n<html>";
        standardStart += "<head>\n";
        standardStart += "<meta charset=\"UTF-8\" />";
        // standardStart += "<title>" + config::config.title + "-" + Html::toSafeHtmlValue(config::config.title) + "</title>";
        standardStart += "<title>" + config::config.title + "</title>";
        standardStart += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/styles/default.min.css\" /><script src=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/highlight.min.js\"></script>  <script>    document.querySelectorAll('code').forEach((code) => {      if (!code.classList.length) {        code.classList.add('language-bash');      }    });  </script>	<script>		document.addEventListener('DOMContentLoaded', (event) => {			hljs.highlightAll();		});	</script>";

        standardStart += "<link rel=\"stylesheet\" href=\"/" + config::config.webPath + "/github.css\">\n";
        standardStart += "<link rel=\"stylesheet\" href=\"/" + config::config.webPath + "/speciou.css\"\n";
        // standardStart += config::config.themeHtml;
        standardStart += "</head>";

        standardStart += "<body>";

        standardStart += "<div class=\"container\">";
        standardStart += "<header class=\"main-header\">";
        standardStart += "<h1 class=\"main-header__title uplize\">";
        standardStart += "<a class=\"main-header__title__link\" href=\"/" + config::config.webPath + "\">" + config::config.title + "</a></h1>";
        standardStart += "<nav class=\"main-header__nav\"><ul class=\"main-nav\"><li class=\"main-nav__list\">";
        standardStart += "<a class=\"main-nav__list__link active\" href=\"/" + config::config.webPath + "\" target=\"_self\">HOME</a>";
        standardStart += "</li><li class=\"main-nav__list\"><a class=\"main-nav__list__link\" href=\"/" + config::config.webPath + "archives/\"target=\"_self\">ARCHIVE</a>";
        standardStart += "</li><li class=\"main-nav__list\"><a class=\"main-nav__list__link\" href=\"https://github.com/Joe1sn\"target=\"_blank\">GITHUB</a></li></ul></nav></header></div>";
        standardStart += "<div id=\"write\" class>\n";

        std::string standardEnd = "</div></body></html>\n";

        //TODO: about me link and more links

        indexPage += standardStart;
        //按照时间排序，约新越靠前
        std::sort(this->allMdFiles.begin(), this->allMdFiles.end(),
            [](std::unique_ptr<markdown::markdown>& a, std::unique_ptr<markdown::markdown>& b) {
                return a->date > b->date;
            }
        );
        size_t counter = 0;
        size_t totalPage = size_t((this->allMdFiles.size() + config::config.per_page - 1) / config::config.per_page);
        fs::path archivePath = this->archiveDir / config::config.pagination_dir;
        FileAction::recursiveCreateDir(fs::relative(archivePath).string(), "\\");

        for (auto& mdFile : this->allMdFiles) {
            counter++;
            //开始生成每个page的html

            //1.生成markdown原始排版
            fs::path p = mdFile->path;
            std::string title = mdFile->articleTile;
            if (mdFile->articleTile.ends_with("\n"))
                title = mdFile->articleTile.substr(0, mdFile->articleTile.length() - 1);
            //1.1 合成标题
            title = Html::replaceAll(title, "[", "\\[");
            title = Html::replaceAll(title, "]", "\\]");
            title = Html::replaceAll(title, "(", "\\(");
            title = Html::replaceAll(title, ")", "\\)");

            mdFormat += "# [" + title + "](/" + config::config.webPath \
                + "/" + config::config.archive_dir + "/" + Html::formatTimestamp(mdFile->date) + \
                "/" + Html::urlEncode(p.stem().string()) + ".html" + ")\n\n";

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

            if (counter % config::config.per_page == 0
                || (counter == allMdFiles.size())) {//保存该html
                //2.1渲染markdown并合成
                size_t currentPage = size_t((counter + config::config.per_page - 1) / config::config.per_page);
                indexPage += cmark_markdown_to_html(mdFormat.c_str(), mdFormat.length(), CMARK_OPT_UNSAFE);
                if (currentPage == 1 && totalPage > 1)//第一页翻页下标
                    indexPage += "<div class=\"pagination\"><a class=\"pagination__link pagination__next\" href=\"/" + config::config.webPath + "/" + config::config.archive_dir + "/page/2.html\">next</a></div>";
                else if (currentPage != totalPage && totalPage > 1)//中间页
                    indexPage += "<div class=\"pagination\"><a class=\"pagination__link pagination__prev\" href=\"/" + config::config.webPath + "/" + config::config.archive_dir + "/page/" + std::to_string(currentPage - 1) + ".html" \
                    + "\">prev</a><a class=\"pagination__link pagination__next\" href=\"/" + config::config.webPath + "/" + config::config.archive_dir + "/page/" + std::to_string(currentPage + 1) + ".html\">next</a></div>";
                else if (totalPage != 1 && currentPage == totalPage) { //尾页
                    indexPage += "<div class=\"pagination\"><a class=\"pagination__link pagination__prev\" href=\"/" + config::config.webPath + "/" + config::config.archive_dir + "/page/" + std::to_string(currentPage - 2) + ".html" \
                        + "\">prev</a></div>";
                }

                indexPage += standardEnd;
                //2.2创建页的路径
                std::ofstream outfile(archivePath / (std::to_string(size_t(counter / config::config.per_page)) + ".html"));
                outfile.write(indexPage.c_str(), indexPage.length());
                outfile.close();

                if (currentPage == 1 && totalPage > 1) {
                    std::ofstream outfile2(config::config.webRootDir / "index.html");
                    outfile2.write(indexPage.c_str(), indexPage.length());
                    outfile2.close();
                }
                indexPage = standardStart;
                mdFormat = "";
            }
        }
        logger::success << "[Archive Index]create successful\n";

    }

    //对所有md文件转到html
    bool website::renderMarkdowns() {
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
        return true;
    }

    //渲染所有路径下的index文件
    bool website::renderIndexPage() {
        return true;
    }

}
#include "include/actions.hpp"
namespace FileAction {
    /*****************************
    *  文件操作类
    *****************************/
    //创建子文件夹
    bool createSubDir(std::string subdir) {
        fs::path dir = subdir;
        if (!fs::exists(dir)) { //该文件夹不存在
            if (fs::create_directory(fs::absolute(dir)))
                logger::success << "Directory:" << fs::absolute(dir) << " created successfully\n";
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
        // std::vector<std::unique_ptr<markdown::markdown>> files = this->allMdFiles;
        //按照时间排序，约新越靠前
        // std::sort(this->allMdFiles.begin(), this->allMdFiles.end(),
        //     [](std::unique_ptr<markdown::markdown>& a, std::unique_ptr<markdown::markdown>& b) {
        //         return a->date > b->date;
        //     }
        // );
        fs::path tempPath;
        std::ofstream fileObj;
        std::string tempStr;
        std::string filename;

        for (auto& file : this->allMdFiles)
        {
            filename = file->path.stem().string() + ".html";
            file->parser();
            tempStr = file->toHtml();

            //TODO: archive by some limit
            // tempPath = this->archiveDir / std::to_string(size_t(pageIndex / config::config.per_page) + 1) / filename;
            tempPath = this->archiveDir / filename;

            if (!fs::exists(tempPath)) {
                FileAction::createSubDir(tempPath.parent_path().string());
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
        catch (const std::exception& e)
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
        catch (const std::exception& e)
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
        standardStart += "<title>" + config::config.title + "-" + markdown::toSafeHtmlValue(config::config.title) + "</title>";
        standardStart += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/styles/default.min.css\" /><script src=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/highlight.min.js\"></script>  <script>    document.querySelectorAll('code').forEach((code) => {      if (!code.classList.length) {        code.classList.add('language-bash');      }    });  </script>	<script>		document.addEventListener('DOMContentLoaded', (event) => {			hljs.highlightAll();		});	</script>";
        standardStart += config::config.themeHtml;
        standardStart += "</head>";

        standardStart += "<body class=\"typora-export os-windows\">";
        //TODO: remove TestRepo
        standardStart += "<a href=\"/TestRepo\" class=\"url\"><center><h1>" + config::config.title + "</center></h1></a>";
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
        for (auto& mdFile : this->allMdFiles) {
            fs::path p = mdFile->path;
            // mdFile->parser();
            //TODO:TestRepo
            std::string title = mdFile->articleTile;
            if (mdFile->articleTile.ends_with("\n"))
                title = mdFile->articleTile.substr(0, mdFile->articleTile.length() - 1);
            mdFormat += "# [" + title + "](\\/TestRepo\\/"\
                + config::config.archive_dir + "\\/" + p.stem().string() + ".html" + ")\n\n";
            // markdown::markdown tempMD(p.string());
            // mdFile->parser();

            mdFormat += mdFile->dateStr + "\n\n";

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
        }

        indexPage += cmark_markdown_to_html(mdFormat.c_str(), mdFormat.length(), CMARK_OPT_UNSAFE);
        indexPage += standardEnd;

        std::ofstream outfile((this->archiveDir / filename));
        std::ofstream outfile2((config::config.webRootDir / filename));
        outfile.write(indexPage.c_str(), indexPage.length());
        outfile2.write(indexPage.c_str(), indexPage.length());
        outfile.close();
        outfile2.close();
        logger::success << "[Archive Index]create successful\n";

        //
        // indexPage.clear();


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
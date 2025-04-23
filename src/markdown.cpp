#include "include/markdown.hpp"


/****
 * MARKDOWN
 */
namespace markdown {
    ////////////////////////////
    //markdown主类
    ////////////////////////////
    markdown::markdown(std::string path) {
        fs::path filePath = path;
        this->path = fs::absolute(filePath);
        if (!fs::exists(filePath)) {
            logger::error << "no such file: " << fs::absolute(path) << "\n";
            throw std::runtime_error("no such file");
        }
        std::ifstream file(filePath, std::ios::in);
        this->rawBuffer << file.rdbuf();
        file.close();
        this->rawData = this->rawBuffer.str();
    }

    //markdown主解析语法
    bool markdown::parser() {
        this->parserYaml();
        this->body = this->rawData.substr(this->yamlPos);
        return true;
    }

    //打印方法
    void markdown::print() {
        this->printFile();
        this->printYaml();
        this->printBody();
    }
    std::ostream& operator<<(std::ostream& os, markdown& obj) {
        obj.print();
        return os;
    }

    //
    std::string markdown::toHtml(std::vector<std::string> cssfiles) {

        auto content = Html::html(config::config.title, config::config.subtitle, "", cssfiles);
        content.bodySetter(this->bodyToHtml());

        return content.to_string();
    }

    //yaml头转为html
    std::string markdown::yamlToHtml() {
        std::string result = "<title>" + config::config.title + "-" + rstr::toSafeHtmlValue(this->articleTile) + "</title>";
        return result;
    }

    //body转为html
    std::string markdown::bodyToHtml() {
        std::string result = "<div id=\"write\" class>\n";
        result += "<p><strong>" + this->dateStr + "</strong></p>\n";
        result += "<p><strong>Tag: </strong>";
        for (auto tag : this->tags)
        {

            std::string tagName = fs::path(tag).stem().string();
            result += "<a href=\"" \
                + config::config.webPath + "/" + config::config.tag_dir + "/"\
                + tagName + "\">" + tagName + " </a>";

        }
        result += "</p>\n";


        result += cmark_markdown_to_html(this->body.c_str(), this->body.length(), CMARK_OPT_UNSAFE);
        result += "\n</div>\n";
        return result;
    }

    // PRIVATE

    //解析YAML
    //注：使用`rawBuffer.seekg(0)`可以回到开头的指针
    bool markdown::parserYaml() {
        std::string singleLine = "";
        ProperType lastProperty = ProperType::EMPTY;
        size_t lineNumber = 1;

        std::getline(this->rawBuffer, singleLine);

        if (!singleLine.starts_with("---")) {
            logger::warn << "file: " << this->path << " don't have valid yaml head,will use default yaml config\n";
            this->dateStr = "1970-01-01 00:00:00";
            try
            {
                this->date = rstr::parseTimeString(this->dateStr);
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            fs::path rootPath = this->categories;
            fs::path tempPath = "default";
            this->categories = fs::absolute((rootPath / tempPath)).string();

            tempPath = this->path;
            this->articleTile = tempPath.stem().string();
            return false;
        }
        while (std::getline(this->rawBuffer, singleLine))
        {
#ifdef LINUX
            singleLine.erase(std::remove(singleLine.begin(), singleLine.end(), '\n'), singleLine.end());
            singleLine.erase(std::remove(singleLine.begin(), singleLine.end(), '\r'), singleLine.end());
#endif
            if (singleLine.starts_with("title:")) {  //文章标题
                singleLine = rstr::getValueAfter("title:", singleLine, true);
                if (!singleLine.empty())
                    this->articleTile = singleLine;
            }
            else if (singleLine.starts_with("date:")) {   //文章日期
                singleLine = rstr::getValueAfter("date:", singleLine, true);
                this->dateStr = singleLine;
                //将字符串解析为时间
                try
                {
                    this->date = rstr::parseTimeString(singleLine);
                }
                catch (const std::exception& e)
                {
                    logger::error << e.what() << '\n';
                    return false;
                }


            }
            else if (singleLine.starts_with("categories:")) {
                lastProperty = ProperType::CATE;    //设置当前级别为目录级别，对'-'进行目录递归处理
            }
            else if (singleLine.starts_with("tags:")) {   //标签
                lastProperty = ProperType::TAG;
            }
            else if (singleLine.starts_with("- ")) {   //子级目录处理
                switch (lastProperty)
                {
                case ProperType::EMPTY: {
                    logger::error << "Line:" << lineNumber << ".wrong yaml head in markdown file: " << this->path << "\n";
                    return false;
                }
                case ProperType::CATE: {
                    singleLine = rstr::getValueAfter("- ", singleLine, true);  //singleLine为目录
                    if (singleLine.empty()) { //
                        logger::error << "Line:" << lineNumber << ".wrong yaml head in markdown file: " << this->path << "\n";
                        return false;
                    }
                    fs::path rootPath = fs::absolute(fs::path(Constant::defaultWebDir) / this->categories);
                    fs::path tempPath = singleLine;

                    this->categories = fs::absolute((rootPath / tempPath)).string();
                    break;
                }
                case ProperType::TAG: {
                    singleLine = rstr::getValueAfter("- ", singleLine, true);  //singleLine为标签
                    if (singleLine.empty()) { //
                        logger::error << "Line:" << lineNumber << ".wrong yaml head in markdown file: " << this->path << "\n";
                        return false;
                    }
                    fs::path rootPath = fs::absolute(fs::path(Constant::defaultWebDir) / config::config.tag_dir);
                    fs::path tempPath = singleLine;
                    this->tags.push_back(fs::absolute((rootPath / tempPath)).string());
                    break;
                }

                default:
                    break;
                }
            }
            else if (singleLine.starts_with("---"))
                break;
            ++lineNumber;
        }
        this->yamlPos = this->rawBuffer.tellg();

        if (this->rawData.find("<!--more-->") == std::string::npos)  //找不到摘要标记
            this->summary = "";
        else {
            while (std::getline(this->rawBuffer, singleLine)) {
                if (singleLine.find("<!--more-->") != std::string::npos) {
                    break;
                }
                this->summary += singleLine;
            }
        }
        this->rawBuffer.seekp(this->yamlPos);

        return true;
    }


    //打印文件基本信息
    void markdown::printFile() {
        logger::debug << "File: " << this->path << "\n";
        logger::debug << "start with: \n=========\n" << this->rawData.substr(0, 0x20) << "\n=========\n";
    }

    //打印头部的yml信息
    void markdown::printYaml() {
        logger::debug << "YAML head finished at " << this->yamlPos << "\n";
        logger::debug << "Title: " << this->articleTile << "\n";
        logger::debug << "date: " << this->dateStr << "\n";
        logger::debug << "Categories: " << this->categories << "\n";
        logger::debug << "Tags:\n";
        for (auto tag : this->tags)
            logger::debug << "    " << tag << "\n";


    }

    //打印部分正文信息
    void markdown::printBody() {
        logger::debug << "---body:---\n";
        logger::debug << this->body.substr(0, 0x20) << "\n";
        logger::debug << "---body finished---\n";

    }

    bool newMarkdownFile(std::string filename) {
        fs::path fp = config::config.postDir;
        fp = fp / (filename + ".md");
        try
        {
            // ---
            // title: test
            // date: 2025-04-23 20:22:26
            // tags:
            // ---

            auto fileObj = std::ofstream(fp);
            fileObj << "---\n";
            fileObj << "title: " << filename << "\n";
            auto tp = std::chrono::system_clock::now();
            auto t = std::chrono::floor<std::chrono::seconds>(tp);
            //Using UTC+0
            fileObj << "date: " << std::format("{:%Y-%m-%d %H:%M:%S}", t) << "\n";
            fileObj << "tags: \n";
            fileObj << "---\n";


            fileObj.close();
            return true;
        }
        catch (const std::exception& e)
        {
            logger::error << e.what() << '\n';
            return false;
        }


    }
}

#include "include/markdown.hpp"


/****
 * MARKDOWN
 */
namespace markdown {
    ////////////////////////////
    //markdown 标签
    //      如： `file` 等
    ////////////////////////////

    /*
    lineElement，派生自element
    不需要包裹的元素，如 #
    */
    //判断是否为不包裹式元素
    bool lineElement::isValid() {
        std::string cleanedBuffer = Html::eraseSpace(this->buffer);
        if (!cleanedBuffer.starts_with(this->pattern))
            return false;
        return true;
    }
    bool lineElement::isValid(std::string src) {
        std::string cleanedBuffer = src;
        if (!this->inParserMod) { //第一次还未进入到识别
            cleanedBuffer = Html::eraseSpace(this->buffer);
            this->inParserMod = true;  //表示进入到识别
        }
        if (!cleanedBuffer.starts_with(this->pattern))
            return false;
        return true;
    }

    //获得元素值
    bool lineElement::parserValue() {
        if (!this->isValid())
            return false;
        this->value = Html::getValueAfter(this->pattern, this->buffer);
        return true;
    }
    //从字符串获得元素值
    bool lineElement::parserValue(std::string src) {
        if (!this->isValid(src))
            return false;
        this->value = Html::getValueAfter(this->pattern, src);
        return true;
    }

    /*
    warppedElement，派生自element
    需要包裹的元素，如 `str`
    */
    //判断是否为包裹式元素
    bool warppedElement::isValid() {
        std::string cleanedBuffer = Html::eraseSpace(this->buffer);
        if (!cleanedBuffer.starts_with(this->pattern)   //以pattern开头和结尾
            || !cleanedBuffer.ends_with(this->pattern))
            return false;
        return true;
    }

    //获得元素值
    bool warppedElement::parserValue() {
        if (!this->isValid())
            return false;
        this->value = Html::getValueBetween(this->pattern, this->pattern, this->buffer);
        return true;
    }

    ////////////////////////////
    //markdown标签
    ////////////////////////////

    /*  #  */
    head::head(std::string& buffer) :lineElement(buffer, "#") {
        if (!this->isValid()) {
            logger::error << "parse head label failed\n";
            throw std::runtime_error("wrong format of head label");
        }

        this->level = 0;
        for (this->level;
            this->parserValue(this->getValue()) \
            && this->level < this->maxLevel;
            this->level++);
    };
    std::string head::toHTML() { //将markdown标签转为html元素
        std::string result = "";
        std::string htmlElem = HTML::header + std::to_string(this->level);
        result = Html::warpHtmlElement(this->getValue(), htmlElem);
        return result;
    }

    /*  -  */
    //TODO: 1. 2. ordered list
    list::list(std::string& buffer, ListType type) :\
        lineElement(buffer, "-"), listType(type) {
        if (!this->isValid()) {
            logger::error << "parse list label failed\n";
            throw std::runtime_error("wrong format of list label");
        }

        this->level = 0;
        for (auto c : this->getValue())
        {
            if (c == ' ' && this->level < (2 * this->maxLevel + 1))
                this->level++;
            else
                break;
        }
        this->level /= 2;

        if (this->level > this->maxLevel)   //safebelt,可能没必要
            this->level = this->maxLevel;

        this->parserValue();

    };
    //该函数只是单纯的标记标签，多层嵌套参考level成员
    std::string list::toHTML() { //将markdown标签转为html元素
        std::string result = "";
        std::string htmlElem = HTML::list;
        result = Html::warpHtmlElement(this->getValue(), htmlElem);
        return result;
    }

    /*  >  */


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
    std::string markdown::toHtml() {
        std::string result = "<!DOCTYPE html>\n<html>";
        result += "<head>\n";
        result += "<meta charset=\"UTF-8\" />";
        result += this->yamlToHtml();
        result += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/styles/default.min.css\" /><script src=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/highlight.min.js\"></script>  <script>    document.querySelectorAll('code').forEach((code) => {      if (!code.classList.length) {        code.classList.add('language-bash');      }    });  </script>	<script>		document.addEventListener('DOMContentLoaded', (event) => {			hljs.highlightAll();		});	</script>";


        result += "<link rel=\"stylesheet\" href=\"/" + config::config.webPath + "/github.css\">\n";
        result += "<link rel=\"stylesheet\" href=\"/" + config::config.webPath + "/speciou.css\"\n";
        result += "</head>";

        result += "<body>";
        result += "<div class=\"container\">";
        result += "<header class=\"main-header\">";
        result += "<h1 class=\"main-header__title uplize\">";
        result += "<a class=\"main-header__title__link\" href=\"/" + config::config.webPath + "\">" + config::config.title + "</a></h1>";
        result += "<nav class=\"main-header__nav\"><ul class=\"main-nav\"><li class=\"main-nav__list\">";
        result += "<a class=\"main-nav__list__link active\" href=\"/" + config::config.webPath + "\" target=\"_self\">HOME</a>";
        result += "</li><li class=\"main-nav__list\"><a class=\"main-nav__list__link\" href=\"/" + config::config.webPath + "archives/\"target=\"_self\">ARCHIVE</a>";
        result += "</li><li class=\"main-nav__list\"><a class=\"main-nav__list__link\" href=\"https://github.com/Joe1sn\"target=\"_blank\">GITHUB</a></li></ul></nav></header></div>";
        result += "<div id=\"write\" class>\n";
        result += "<div id=\"write\" class>\n";


        result += "<div class=\"typora-export os-windows\">";
        result += this->bodyToHtml();
        result += "</div></body>";
        return result;
    }

    //yaml头转为html
    std::string markdown::yamlToHtml() {
        std::string result = "<title>" + config::config.title + "-" + Html::toSafeHtmlValue(this->articleTile) + "</title>";
        return result;
    }

    //body转为html
    std::string markdown::bodyToHtml() {
        std::string result = "<div id=\"write\" class>\n";
        result += "<p><strong>" + this->dateStr + "</strong></p>\n";
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
                this->date = Html::parseTimeString(this->dateStr);
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
                singleLine = Html::getValueAfter("title:", singleLine, true);
                if (!singleLine.empty())
                    this->articleTile = singleLine;
            }
            else if (singleLine.starts_with("date:")) {   //文章日期
                singleLine = Html::getValueAfter("date:", singleLine, true);
                this->dateStr = singleLine;
                //将字符串解析为时间
                try
                {
                    this->date = Html::parseTimeString(singleLine);
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
                    singleLine = Html::getValueAfter("- ", singleLine, true);  //singleLine为目录
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
                    singleLine = Html::getValueAfter("- ", singleLine, true);  //singleLine为标签
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




}

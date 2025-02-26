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
        std::string cleanedBuffer = eraseSpace(this->buffer);
        if (!cleanedBuffer.starts_with(this->pattern))
            return false;
        return true;
    }
    bool lineElement::isValid(std::string src) {
        std::string cleanedBuffer = src;
        if (!this->inParserMod) { //第一次还未进入到识别
            cleanedBuffer = eraseSpace(this->buffer);
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
        this->value = getValueAfter(this->pattern, this->buffer);
        return true;
    }
    //从字符串获得元素值
    bool lineElement::parserValue(std::string src) {
        if (!this->isValid(src))
            return false;
        this->value = getValueAfter(this->pattern, src);
        return true;
    }

    /*
    warppedElement，派生自element
    需要包裹的元素，如 `str`
    */
    //判断是否为包裹式元素
    bool warppedElement::isValid() {
        std::string cleanedBuffer = eraseSpace(this->buffer);
        if (!cleanedBuffer.starts_with(this->pattern)   //以pattern开头和结尾
            || !cleanedBuffer.ends_with(this->pattern))
            return false;
        return true;
    }

    //获得元素值
    bool warppedElement::parserValue() {
        if (!this->isValid())
            return false;
        this->value = getValueBetween(this->pattern, this->pattern, this->buffer);
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
        result = warpHtmlElement(this->getValue(), htmlElem);
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
        result = warpHtmlElement(this->getValue(), htmlElem);
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
        std::string result = "<!DOCTYPE html>\n";
        result += "<head>\n";
        result += "<meta charset=\"UTF-8\" />";
        result += this->yamlToHtml();
        result += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/styles/default.min.css\" /><script src=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/highlight.min.js\"></script>  <script>    document.querySelectorAll('code').forEach((code) => {      if (!code.classList.length) {        code.classList.add('language-bash');      }    });  </script>	<script>		document.addEventListener('DOMContentLoaded', (event) => {			hljs.highlightAll();		});	</script>";
        result += config::config.themeHtml;
        result += "</head>";

        result += "<body class=\"typora-export os-windows\">";
        //TODO: remove repo
        result += "<a href=\"/TestRepo\" class=\"url\"><center><h1>" + config::config.title + "</center></h1></a>";
        result += this->bodyToHtml();
        result += "</body>";
        return result;
    }

    //yaml头转为html
    std::string markdown::yamlToHtml() {
        std::string result = "<title>" + config::config.title + "-" + toSafeHtmlValue(this->articleTile) + "</title>";
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
                this->date = parseTimeString(this->dateStr);
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
                singleLine = getValueAfter("title:", singleLine, true);
                if (!singleLine.empty())
                    this->articleTile = singleLine;
            }
            else if (singleLine.starts_with("date:")) {   //文章日期
                singleLine = getValueAfter("date:", singleLine, true);
                this->dateStr = singleLine;
                //将字符串解析为时间
                try
                {
                    this->date = parseTimeString(singleLine);
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
                    singleLine = getValueAfter("- ", singleLine, true);  //singleLine为目录
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
                    singleLine = getValueAfter("- ", singleLine, true);  //singleLine为标签
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


    ////////////////////////////
    //字符串辅助函数
    ////////////////////////////

    //查找Pattern后的值
    std::string getValueAfter(const std::string pattern, const std::string& input, bool needClean) {
        std::string result = "";
        size_t prefixPos = input.find(pattern);
        if (prefixPos == std::string::npos) {
            return "";// 如果找不到 pattern，返回空字符串
        }
        size_t titleStart = prefixPos + pattern.length(); // 计算标题的起始位置
        result = input.substr(titleStart);
        if (needClean)
            return eraseSpace(result);
        else
            return result;
    }

    //得到start和end之间的值
    std::string getValueBetween(const std::string start, const std::string end, const std::string& input) {
        //1.先找到start
        std::string result = "";
        size_t prefixPos = input.find(start);
        if (prefixPos == std::string::npos) {
            return "";// 如果找不到 start，返回空字符串
        }
        size_t titleStart = prefixPos + start.length(); // 计算标题的起始位置
        result = input.substr(titleStart);

        //2.剔除end
        size_t postfixPos = result.find(end);
        if (postfixPos == std::string::npos) {
            return "";// 如果找不到 end，返回空字符串
        }
        result = result.substr(0, postfixPos);
        return result;
    }

    //删除字符串最前面和最后面的空格
    std::string eraseSpace(std::string str) {
        // 找到第一个非空格字符的位置
        size_t first = str.find_first_not_of(' ');
        if (first == std::string::npos) {
            return ""; // 如果字符串全是空格，返回空字符串
        }

        size_t last = str.find_last_not_of(' ');// 找到最后一个非空格字符的位置

        return str.substr(first, last - first + 1);// 返回去掉前后空格后的子字符串
    }

    //将时间字符串解析为 std::chrono::seconds
    std::chrono::seconds parseTimeString(const std::string& timeStr) {

        // 使用 std::tm 结构存储解析后的时间
        std::tm tm = {};

        std::istringstream ss(timeStr);
        ss >> std::get_time(&tm, Constant::timeFormat);

        if (ss.fail()) {
            throw std::runtime_error("Failed to parse time string");
        }
        std::time_t time = std::mktime(&tm);

        return std::chrono::seconds(time);
    }

    //替换所有字符串
    //对 `str` 中的所有 `from` 替换为 `to`
    std::string replaceAll(const std::string str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t startPos = 0;
        while ((startPos = result.find(from, startPos)) != std::string::npos) {
            result.replace(startPos, from.length(), to);
            startPos += to.length(); // 避免无限循环
        }
        return result;
    }


    //HTML相关
    //将字符串转为安全的html字符，避免xss
    std::string toSafeHtmlValue(std::string src) {
        src = replaceAll(src, "&", "&amp;");
        src = replaceAll(src, "<", "&lt;");
        src = replaceAll(src, ">", "&gt;");
        src = replaceAll(src, "\"", "&quot;");
        src = replaceAll(src, "'", "&apos;");
        // src = replaceAll(src, " ", "&nbsp;");  // 空格替换为 &nbsp;，如果需要
        // src = replaceAll(src, "\n", "<br>");  // 换行符替换为 <br> 标签
        return src;
    }

    //使用html包裹值
    //value: html内的值
    //htmlElement: 裸html元素，如<p>的htmlElement就为：p
    //htmlAtrribute: html元素的相关属性，如<a href="www.example.com">中的 href="www.example.com"
    std::string warpHtmlElement(std::string value, std::string htmlElement, std::vector<std::string > htmlAtrribute) {
        //1.合成html元素
        std::string startElem = "<" + htmlElement;
        for (auto config : htmlAtrribute)
            startElem += " " + config;
        startElem += ">";
        std::string endElem = "</" + htmlElement + ">";

        //2.清洗value并合成结果
        value = toSafeHtmlValue(value);
        return startElem + value + endElem;
    }


}

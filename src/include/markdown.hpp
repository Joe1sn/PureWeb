/**
 * @file markdown.hpp
 * @author Joe1sn (joe1sn23333@gmail.com)
 * @brief markdown解析器
 * @version 0.1
 * @date 2025-02-21
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include "log.hpp"
#include "config.hpp"
#include "constant.hpp"
#include "html.hpp"

#include <cmark.h>

#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;
namespace markdown {
    /***************
    //markdown元素
    ***************/
    class element   //元素基类
    {
    public:
        enum class ElementType { LINE, WARP, MISC };
        virtual ~element() = default;
        virtual bool isValid() = 0;   //该md元素是否合法
        virtual bool parserValue() = 0;   //根据元素获得对应值，如 # Head，值为 `Head`
    };

    //不需要包裹的元素，如 #
    class lineElement : public element {

    private:
        std::string buffer;
        std::string pattern;
        std::string value;
        bool inParserMod;
        ElementType type = ElementType::LINE;
    public:
        lineElement(std::string& buffer, std::string& pattern)
            : buffer(buffer), value(buffer), pattern(pattern), inParserMod(false) {
        };
        lineElement(std::string& buffer, const char* pattern)
            : buffer(buffer), value(buffer), pattern(pattern), inParserMod(false) {
        };

        std::string getValue() { return this->value; };    //获得value的API
        std::string getPattern() { return this->pattern; };    //获得pattern的API
        ElementType getType() { return this->type; };    //获得type的API

        bool isValid() override;
        bool isValid(std::string src);
        bool parserValue() override;
        bool parserValue(std::string src);

    };

    //需要包裹的元素，如 `str`
    class warppedElement : public element {
    private:
        std::string buffer;
        std::string pattern;
        std::string value;

        ElementType type = ElementType::WARP;
    public:
        warppedElement(std::string& buffer, std::string& pattern)
            : buffer(buffer), pattern(pattern) {
        };  //由于元素标记为对称样式，所以pattern仅为一般即可，如`str`的pattern就为 `
        warppedElement(std::string& buffer, const char* pattern)
            : buffer(buffer), pattern(pattern) {
        };  //由于元素标记为对称样式，所以pattern仅为一般即可，如`str`的pattern就为 `

        std::string getValue() { return this->value; };    //获得value的API
        std::string getPattern() { return this->pattern; };    //获得pattern的API
        ElementType getType() { return this->type; };    //获得type的API

        bool isValid() override;
        bool parserValue() override;

    };

    //其他元素，如 表格类型等
    //需要孙子类自行设计isValid和paser
    class miscElement : public element {
    private:
        std::string buffer;

        ElementType type = ElementType::MISC;
    public:
        miscElement(std::string& buffer)
            : buffer(buffer) {
        };

        std::string getBuffer() { return this->buffer; };    //获得buffer的API
        ElementType getType() { return this->type; };    //获得type的API

    };


    /***************
    //markdown标签
    ***************/
    class head : public lineElement {
    public:
        size_t level;   //0表示没有，最大值为6
    private:
        const size_t maxLevel = 6;  //最大层数
    public:
        head(std::string& buffer);
        std::string toHTML();
    };

    class list : public lineElement {
    public:
        enum class ListType { ORDER, DISORDER };
        size_t level;   //0表示没有，最大值为3
        ListType listType;
    private:
        const size_t maxLevel = 3;  //最大层数
    public:
        list(std::string& buffer, ListType type);
        std::string toHTML();
    };

    /***************
    //markdown主类
    ***************/
    class markdown
    {

    public:
        enum class ProperType { EMPTY, CATE, TAG };

        //从yml头获得的信息
        std::string articleTile = "test";    //文章题目
        std::chrono::seconds date;  //文章时间-时间
        std::string dateStr;  //文章时间-字符串
        // std::string categories = config::config.category_dir;    //所属目录
        std::string categories = config::config.category_dir;
        std::vector<std::string> tags = {  };  //标签
        std::string body;   //真正的markdown文章
        std::string summary;    //摘要
        fs::path path;    //原始文本路径

    private:
        std::string rawData;    //原始文本
        std::stringstream rawBuffer;
        std::streampos yamlPos; //读取完yaml头的流指针位置

    public:
        markdown(std::string path);
        ~markdown() {};

        bool parser();
        void print();
        friend std::ostream& operator<<(std::ostream& os, markdown& obj);

        std::string toHtml();
        std::string yamlToHtml();
        std::string bodyToHtml();

    private:
        bool parserYaml();

        void printFile();
        void printYaml();
        void printBody();
    };


    // std::string getValueAfter(const std::string pattern, const std::string& input, bool needClean = false);//获得之后的值
    // std::string getValueBefore(const std::string pattern, const std::string& input, bool needClean = false);//获得之前的值
    // std::string getValueBetween(const std::string start, const std::string end, const std::string& input);//获得之间的值
    // std::string eraseSpace(std::string str);//清楚最前面和最后面的空格
    // std::chrono::seconds parseTimeString(const std::string& timeStr);//2024-1-1 10:00:00这样的时间字符串转为时间戳
    // std::string replaceAll(const std::string str, const std::string& from, const std::string& to);  //替换所有
    // std::vector<std::string> splitStr(const std::string str, const std::string pattern);//按照pattern分割str

    // std::string toSafeHtmlValue(std::string src);  //SECURE: Avoid XSS
    // std::string warpHtmlElement(std::string value, std::string htmlElement, std::vector<std::string > htmlAtrribute = {});
}
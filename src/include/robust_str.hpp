#pragma once
#include "constant.hpp"
#include "log.hpp"
#include "config.hpp"
// #include "actions.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <fstream>

namespace rstr
{
    std::string getValueAfter(const std::string pattern, const std::string& input, bool needClean = false);//获得之后的值
    std::string getValueBefore(const std::string pattern, const std::string& input, bool needClean = false);//获得之前的值
    std::string getValueBetween(const std::string start, const std::string end, const std::string& input);//获得之间的值
    std::string eraseSpace(std::string str);//清楚最前面和最后面的空格

    std::chrono::seconds parseTimeString(const std::string& timeStr);//2024-1-1 10:00:00这样的时间字符串转为时间戳
    std::string formatTimestamp(const std::chrono::seconds tp, std::string fmt = "");
    std::string replaceAll(const std::string str, const std::string& from, const std::string& to);  //替换所有
    std::vector<std::string> splitStr(const std::string str, const std::string pattern);//按照pattern分割str


    std::string toSafeHtmlValue(std::string src);  //SECURE: Avoid XSS
    std::string warpHtmlElement(std::string value, std::string htmlElement, std::vector<std::string > htmlAtrribute = {});
    std::string urlEncode(const std::string value);
}

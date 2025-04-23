#include "include/robust_str.hpp"

namespace rstr {
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

    //获得之前的值
    std::string getValueBefore(const std::string pattern, const std::string& input, bool needClean) {
        std::string result = "";
        size_t prefixPos = input.find(pattern);// 计算标题的起始位置
        if (prefixPos == std::string::npos) {
            return "";// 如果找不到 pattern，返回空字符串
        }
        result = input.substr(0, prefixPos);
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

    //将std::chrono::seconds解析为 Year/Mouth/Day
    std::string formatTimestamp(const std::chrono::seconds tp, std::string fmt)
    {
        auto t = std::chrono::system_clock::time_point(tp);
        // 转换为 year/month/day 格式
        if (fmt == "")
            return std::format("{:%Y/%m/%d}", t);
        else
            return std::format("{:%Y-%m-%d %H:%M:%S}", t);//stupid c++
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

    //按照pattern分割str
    std::vector<std::string> splitStr(const std::string str, const std::string pattern) {
        std::vector<std::string> result = {};
        std::string tempStr = "";
        std::string remain = str;
        for (;(tempStr = getValueBefore(pattern, remain)) != "";)
        {
            result.push_back(tempStr);
            remain = remain.substr(tempStr.length() + pattern.length());
            tempStr = "";
        }
        result.push_back(remain);
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

    //转为url编码
    std::string urlEncode(const std::string value) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (unsigned char c : value) {
            // 保留安全字符
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            }
            // 编码其他字符
            else {
                escaped << '%' << std::setw(2) << (int)c;
            }
        }

        return escaped.str();
    }
}
/**
 * @file log.cpp
 * @author Joe1sn (joe1sn23333@gmail.com)
 * @brief   日志打印
 * @version 0.1
 * @date 2025-02-19
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "include/log.hpp"


namespace logger {
    /*****************************
     *  打印色彩
     *****************************/
    void set_color(charColor color) {
#ifdef WINDOWS
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color.fore | color.back);
#elif defined(LINUX)
        std::cout << "\033[" << std::hex << color.fore << "m";
#endif
    }

    void set_normal() {
#ifdef WINDOWS
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (!orignColor.back && !orignColor.fore)
            SetConsoleTextAttribute(
                GetStdHandle(STD_OUTPUT_HANDLE),
                FOREGROUND_INTENSITY |
                FOREGROUND_RED |
                FOREGROUND_GREEN |
                FOREGROUND_BLUE);
        else
            SetConsoleTextAttribute(
                GetStdHandle(STD_OUTPUT_HANDLE),
                orignColor.color());
#elif defined(LINUX)
        std::cout << "\033[0m";
#endif        
    }

    charColor get_color() {
        charColor result = { 0,0 };
#ifdef WINDOWS
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            WORD attributes = csbi.wAttributes;
            result.fore = attributes & 0x0F;
            result.back = (attributes >> 4) & 0x0F;
        }
#elif defined(LINUX)
        //TODO
#endif
        return result;
    }



    /*****************************
     *  输出流
     *****************************/
     /////////             LogStreamBuffer类

     // LogStreamBuffer 构造函数
    LogStream::LogStreamBuffer::LogStreamBuffer(LogLevel level) : level(level) {}

    // LogStreamBuffer 的 overflow 实现
    int LogStream::LogStreamBuffer::overflow(int c) {
        static bool isFirstPrint = true;
        if (isFirstPrint) {
            std::cout << this->getPrefix();
            isFirstPrint = false;
        }
        if (c != EOF)
            std::cout.put(c);  // 输出到控制台
        if (c == '\n') {
            isFirstPrint = true;
            set_normal();
        }
        return c;
    }

    // 获取不同日志等级的前缀
    std::string LogStream::LogStreamBuffer::getPrefix() const {
#ifdef WINDOWS
        switch (level) {
        case LogLevel::DEBUG: {
            set_color(charColor{ FOREGROUND_BLUE, FOREGROUND_INTENSITY });
            return "[*] ";
        }

        case LogLevel::WARN: {
            set_color(charColor{ FOREGROUND_GREEN | FOREGROUND_RED, FOREGROUND_INTENSITY });
            return "[!] ";
        }
        case LogLevel::LERROR: {
            set_color(charColor{ FOREGROUND_RED, FOREGROUND_INTENSITY });
            return "[x] ";
        }
        case LogLevel::SUCCESS: {
            set_color(charColor{ FOREGROUND_GREEN, FOREGROUND_INTENSITY });
            return "[+] ";
        }
        default: return "[?] ";
        }
#elif defined(LINUX)
        switch (level) {
        case LogLevel::DEBUG: {
            set_color(charColor{ 0x34, 0 });
            return "[*] ";
        }

        case LogLevel::WARN: {
            set_color(charColor{ 0x33, 0 });
            return "[!] ";
        }
        case LogLevel::LERROR: {
            set_color(charColor{ 0x31, 0 });
            return "[x] ";
        }
        case LogLevel::SUCCESS: {
            set_color(charColor{ 0x32, 0 });
            return "[+] ";
        }
        default: return "[?] ";
        }
#endif
    }

    // LogStream 构造函数
    LogStream::LogStream(LogLevel level) : std::ostream(&buffer), buffer(level) {}

    // LogStream 析构函数
    LogStream::~LogStream() {}

    /////////             工厂类
    LogStream LogFactory::debug() {
        return LogStream(LogStream::LogLevel::DEBUG);
    }

    LogStream LogFactory::warn() {
        return LogStream(LogStream::LogLevel::WARN);
    }

    LogStream LogFactory::error() {
        return LogStream(LogStream::LogLevel::LERROR);
    }

    LogStream LogFactory::success() {
        return LogStream(LogStream::LogLevel::SUCCESS);
    }

}
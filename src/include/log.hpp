/**
 * @file log.hpp
 * @author Joe1sn (joe1sn23333@gmail.com)
 * @brief   日志打印
 * @version 0.1
 * @date 2025-02-19
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#ifdef WINDOWS
#include <windows.h>
#elif defined(LINUX)
#endif

namespace logger
{
    /*****************************
     *  打印色彩
     *****************************/
    struct charColor
    {
        unsigned short fore, back;
        unsigned short color() {
            return fore | back;
        }
    };


    //设置颜色
    void set_color(charColor color);

    //重置颜色
    void set_normal();

    //获得颜色
    charColor get_color();

    inline charColor orignColor = get_color();

    // 继承自 std::ostream，实现不同的输出流
    class LogStream : public std::ostream {
    public:
        enum class LogLevel { DEBUG, WARN, LERROR, SUCCESS };

        // 构造函数
        LogStream(LogLevel level);
        ~LogStream();

    private:
        // 定义流缓冲区
        class LogStreamBuffer : public std::streambuf {
        public:
            LogStreamBuffer(LogLevel level);

        protected:
            int overflow(int c) override;

        private:
            LogLevel level;
            std::string getPrefix() const;  // 获取不同等级的前缀
        };

        LogStreamBuffer buffer;  // 缓冲区
    };

    // 工厂类，用于生成不同级别的日志输出流
    class LogFactory {
    public:
        //static使用全局单例
        static LogStream debug();
        static LogStream warn();
        static LogStream error();
        static LogStream success();
    };

    inline logger::LogStream debug = logger::LogFactory::debug();
    inline logger::LogStream warn = logger::LogFactory::warn();
    inline logger::LogStream error = logger::LogFactory::error();
    inline logger::LogStream success = logger::LogFactory::success();
} // namespace log

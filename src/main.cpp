#include "include/log.hpp"
#include "include/config.hpp"
#include "include/actions.hpp"
#include "include/markdown.hpp"
#include <cmark.h>

#include <nlohmann/json.hpp>
#include <iostream>

#ifdef WINDOWS
#include <windows.h>
#elif defined(LINUX)
#endif

using json = nlohmann::json;

void debug_test();


int main(int argc, char* argv[]) {
#ifdef WINDOWS
    SetConsoleOutputCP(65001);
#endif

    // debug_test();
    // action::renderMarkdowns();


    std::string temp = "";
    auto website = action::website();
    // if (argc == 1) {
    //     website.renderMarkdowns();
    //     return 0;
    // }

    for (size_t i = 1; i < argc; i++)
    {
        temp = argv[i];
        if (temp == "init")
            website.initWebsite();
        else if (temp == "refresh")
            website.renderMarkdowns();
    }

    // std::string temp = "test/1/2/3/4";
    // for (auto s : Html::splitStr(temp, "/"))
    // {
    //     logger::debug << s << "<";
    // }
    // FileAction::recursiveCreateDir("test/1/2");
    return 0;
}

void debug_test() {

    std::cout << "+++++++++logger output Test+++++++++\n";
    logger::debug << "this is debug\n";
    logger::warn << "this is warning\n";
    logger::error << "this is error\n";
    logger::success << "this is success\n";

    std::cout << "+++++++++Config File Test+++++++++\n";
    config::config.printConfig();

    std::cout << "+++++++++Markdown File Test+++++++++\n";
    auto md = markdown::markdown("D:\\Github\\PureWeb\\doc\\frist_post.md");
    md.parser();
    logger::debug << md;
    logger::debug << "body to html:\n";
    std::cout << md.bodyToHtml() << "\n";


    std::cout << "+++++++++Markdown Parser Test+++++++++\n";
    std::string tempString = "## #this is head";
    auto header = markdown::head(tempString);
    logger::debug << "value: " << header.getValue() << "  level: " << header.level << "\n";
    logger::debug << "to html: " << header.toHTML() << "\n";

    // std::cout << "+++++++++HTML Safty Test+++++++++\n";
    // std::string htmlStr = "#include<iostream>&1-1=0\\\"aa";
    // logger::debug << "to safe html: " << markdown::toSafeHtmlValue(htmlStr) << "\n";


}
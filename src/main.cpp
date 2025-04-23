#include "include/log.hpp"
#include "include/config.hpp"
#include "include/web_actions.hpp"
#include "include/markdown.hpp"
#include <cmark.h>

#include <nlohmann/json.hpp>
#include <iostream>

#ifdef WINDOWS
#include <windows.h>
#elif defined(LINUX)
#endif

using json = nlohmann::json;


int main(int argc, char* argv[]) {
#ifdef WINDOWS
    SetConsoleOutputCP(65001);
#endif

    // debug_test();
    // action::renderMarkdowns();


    std::string temp = "";
    auto website = action::website();
    bool intoSubArg = false;
    std::string lastArg = "";
    for (size_t i = 1; i < argc; i++)
    {
        temp = argv[i];
        if (temp == "init")
            website.initWebsite();
        else if (temp == "refresh")
            website.renderMarkdowns();
        else if (temp == "update")
            gitcmd::updateRepo();
        else if (temp == "new") {
            lastArg = temp;
            intoSubArg = true;
        }
        else if (intoSubArg) {
            intoSubArg = false;
            if (lastArg == "new") {
                if (markdown::newMarkdownFile(temp))
                    logger::success << "created: " << temp << "\n";
                else
                    logger::error << "created: " << temp << "Failed\n";
            }
        }
    }
    return 0;
}
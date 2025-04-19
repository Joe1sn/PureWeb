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

namespace Html
{
    /*****
     * 所有页面默认page
     */
    class html
    {
    private:
        std::string webPath = "";
        std::string standardStart = "";
        std::string standardEnd = "";
        std::string body = "";
    public:
        html(std::string webtitle, std::string title, std::string body_content, std::vector<std::string> styleCssList);
        std::string to_string();
        void bodySetter(std::string);
        ~html() {};
    };


} // namespace Html

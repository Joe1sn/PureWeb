#include "include/html.hpp"

namespace Html {
    html::html(std::string webtitle, std::string title, std::string body_content, std::vector<std::string> styleCssList) {
        this->body = body_content;
        if (config::config.webPath == "" || config::config.webPath == "/")
            this->webPath = "/";
        else {
            if (config::config.webPath[0] != '/')
                this->webPath = "/" + config::config.webPath;
            else
                this->webPath = config::config.webPath;
        }

        if (!this->webPath.ends_with("/")) {
            this->webPath = this->webPath + "/";
        }


        this->standardStart = "<!DOCTYPE html>\n<html>";
        this->standardStart += "<head>\n";
        this->standardStart += "<meta charset=\"UTF-8\" />";
        this->standardStart += "<title>" + webtitle + "</title>";
        this->standardStart += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/styles/default.min.css\" /><script src=\"https://cdnjs.cloudflare.com/ajax/libs/highlight.js/11.8.0/highlight.min.js\"></script>  <script>    document.querySelectorAll('code').forEach((code) => {      if (!code.classList.length) {        code.classList.add('language-bash');      }    });  </script>	<script>		document.addEventListener('DOMContentLoaded', (event) => {			hljs.highlightAll();		});	</script>";
        for (auto cssFile : styleCssList)
            this->standardStart += "<link rel=\"stylesheet\" href=\"" + this->webPath + cssFile + "\">";


        this->standardStart += "</head>\n";
        this->standardStart += "<body>";

        this->standardStart += "<div class=\"container\">";
        this->standardStart += "<header class=\"main-header\">";
        this->standardStart += "<h1 class=\"main-header__title uplize\">";
        this->standardStart += "<a class=\"main-header__title__link\" href=\"" + this->webPath + "\">" + title + "</a></h1>";
        this->standardStart += "<nav class=\"main-header__nav\"><ul class=\"main-nav\"><li class=\"main-nav__list\">";
        this->standardStart += "<a class=\"main-nav__list__link active\" href=\"" + this->webPath + "\" target=\"_self\">HOME</a>";
        this->standardStart += "</li><li class=\"main-nav__list\"><a class=\"main-nav__list__link\" href=\"" + this->webPath + "archives/\"target=\"_self\">ARCHIVE</a>";
        this->standardStart += "</li><li class=\"main-nav__list\"><a class=\"main-nav__list__link\" href=\"https://github.com/Joe1sn\"target=\"_blank\">GITHUB</a></li></ul></nav></header></div>";
        this->standardStart += "<div id=\"write\" class>\n";

        this->standardEnd = "</div></body></html>\n";

    }

    std::string html::to_string() {
        return this->standardStart + this->body + this->standardEnd;
    }

    void html::bodySetter(std::string newContent) {
        this->body = newContent;
    }
}
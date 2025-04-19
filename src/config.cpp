#include "include/config.hpp"


namespace config {
    /*****
     * 配置文件类
     *****/

    configPrototype::configPrototype(std::string confPath)
    {

        if (fs::exists(confPath)) { //如果配置文件存在
            std::ifstream configFile(confPath); //打开config文件
            std::stringstream stream;
            stream << configFile.rdbuf();
            this->rawData = stream.str();
            this->jsonConfig = json::parse(this->rawData);
            logger::success << "load config successfully!\n";

        }
        else {  //配置文件不存在
            logger::error << "can't open file: " << confPath << "\n";
            logger::warn << "now create config file: " << confPath << "\n";
            this->rawData = Constant::defaultConfig;
            this->jsonConfig = json::parse(this->rawData);

            std::ofstream file(confPath);   //生成默认配置文件
            if (file.is_open()) {
                // 将 JSON 数据转换为字符串并写入文件
                file << this->jsonConfig.dump(4);
                file.close();
                logger::success << "config written to file successfully!\n";
            }
            else {
                logger::error << "Failed to create config file: " << confPath << "\n";
                throw std::runtime_error("invalid config file path");
            }
        }

        //检查json格式
        if (!this->checkConfig()) {
            logger::error << "config format is wrong!\n";
            throw std::runtime_error("config file format is wrong!");
        }
        else
            logger::success << "config format check is pass\n";

        this->parserConfig();

        this->postDir = fs::absolute(fs::path(Constant::defaultPostDir));
        this->webRootDir = fs::absolute(fs::path(Constant::defaultWebDir));
    }

    //私有: 解析配置文件
    //2025-4-1: fix: 配置文件可能造成xss
    void configPrototype::parserConfig() {
        this->title = rstr::toSafeHtmlValue(this->jsonConfig["Site"]["title"]);
        this->subtitle = rstr::toSafeHtmlValue(this->jsonConfig["Site"]["subtitle"]);
        this->author = rstr::toSafeHtmlValue(this->jsonConfig["Site"]["author"]);

        std::string tempPath = rstr::toSafeHtmlValue(this->jsonConfig["Site"]["path"]);
        if (tempPath == "")//确定html中的web根目录表现方式唯一
            this->webPath = "/";
        else {
            if (tempPath[0] != '/')
                this->webPath = "/" + tempPath;
            else
                this->webPath = tempPath;
        }
        if (!this->webPath.ends_with("/")) {
            this->webPath = this->webPath + "/";
        }


        this->source_dir = rstr::toSafeHtmlValue(this->jsonConfig["Directory"]["source_dir"]);
        this->public_dir = rstr::toSafeHtmlValue(this->jsonConfig["Directory"]["public_dir"]);
        this->tag_dir = rstr::toSafeHtmlValue(this->jsonConfig["Directory"]["tag_dir"]);
        this->archive_dir = rstr::toSafeHtmlValue(this->jsonConfig["Directory"]["archive_dir"]);
        this->category_dir = rstr::toSafeHtmlValue(this->jsonConfig["Directory"]["category_dir"]);

        this->per_page = this->jsonConfig["Pagination"]["per_page"];
        this->pagination_dir = rstr::toSafeHtmlValue(this->jsonConfig["Pagination"]["pagination_dir"]);

        this->repo = rstr::toSafeHtmlValue(this->jsonConfig["Deploy"]["repo"]);
        this->branch = rstr::toSafeHtmlValue(this->jsonConfig["Deploy"]["branch"]);

    }

    //私有：检查配置文件
    bool configPrototype::checkConfig() {
        if (this->jsonConfig.contains("Site")
            && this->jsonConfig.contains("Directory")
            && this->jsonConfig.contains("Pagination")
            && this->jsonConfig.contains("Deploy")
            ) {

            //1.检查Site选项
            json site = this->jsonConfig["Site"];
            if (!site.contains("title")
                || !site.contains("subtitle")
                || !site.contains("author"))
                return false;
            //2.检查Directory
            site = this->jsonConfig["Directory"];
            if (!site.contains("source_dir")
                || !site.contains("public_dir")
                || !site.contains("tag_dir")
                || !site.contains("archive_dir")
                || !site.contains("category_dir"))
                return false;

            //3.检查Pagination
            site = this->jsonConfig["Pagination"];
            if (!site.contains("per_page")
                || !site.contains("pagination_dir"))
                return false;
            //3.检查Deploy
            site = this->jsonConfig["Deploy"];
            if (!site.contains("repo")
                || !site.contains("branch"))
                return false;

            return true;
        }
        else
            return false;
    }

    //私有：打印json配置文件
    void configPrototype::printConfig() {
        logger::debug << "==========WebSite Config==========\n";

        logger::debug << "Site\n";
        logger::debug << "  title: " << this->title << "\n";
        logger::debug << "  subtitle: " << this->subtitle << "\n";
        logger::debug << "  author: " << this->author << "\n";

        logger::debug << "Directory\n";
        logger::debug << "  source_dir: " << this->source_dir << "\n";
        logger::debug << "  public_dir: " << this->public_dir << "\n";
        logger::debug << "  tag_dir: " << this->tag_dir << "\n";
        logger::debug << "  archive_dir: " << this->archive_dir << "\n";
        logger::debug << "  category_dir: " << this->category_dir << "\n";

        logger::debug << "Pagination\n";
        logger::debug << "  per_page: " << this->per_page << "\n";
        logger::debug << "  pagination_dir: " << this->pagination_dir << "\n";

        logger::debug << "Deploy\n";
        logger::debug << "  repo: " << this->repo << "\n";
        logger::debug << "  branch: " << this->branch << "\n";
        logger::debug << "==================================\n";

    }


}
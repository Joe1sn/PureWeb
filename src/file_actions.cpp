#include "./include/file_actions.hpp"

namespace FileAction {
    /*****************************
    *  文件操作类
    *****************************/
    //创建子文件夹
    bool createSubDir(std::string subdir, bool debug) {
        fs::path dir = subdir;
        if (!fs::exists(dir)) { //该文件夹不存在
            if (fs::create_directory(fs::absolute(dir))) {
                if (debug)
                    logger::success << "Directory:" << fs::absolute(dir) << " created successfully\n";
            }

            else {
                logger::error << "Failed to create directory: " << fs::absolute(dir) << "\n";
                return false;
            }
            return true;
        }
        else {
            // logger::warn << "Directory:" << fs::absolute(dir) << " already exists.\n";
            return true;
        }
    }

    //递归式创建文件夹
    bool recursiveCreateDir(std::string recur_dir, std::string pagin_str) {
        fs::path path = "";
        for (const auto str : rstr::splitStr(recur_dir, pagin_str))
        {
            path = path / str;
            if (!createSubDir(path.string())) { //及时退出避免死循环
                logger::error << "can't create sub directoty: " << str << "\n";
                return false;
            }
        }
        return true;
    }

    //遍历目录下所有文件
    //checker为路径判断的方式
    std::vector<fs::path> getFiles(std::string dir, std::function<bool(fs::path)>checker) {
        std::vector<fs::path> result = {};
        if (!fs::exists(dir)) {
            logger::error << "No such directory: " << dir << "\n";
            return result;
        }
        fs::path root = dir;
        // 遍历目录
        for (const auto& entry : fs::recursive_directory_iterator(root)) {
            if (fs::is_regular_file(entry)) { // 检查是否为普通文件
                if (checker(entry))             //使用checker检查
                    result.push_back(entry);
            }
        }
        return result;
    }

    //根据后缀名选择文件夹下的文件
    std::vector<fs::path> getFiles(std::string dir, std::string postfix) {
        std::vector<fs::path> result = {};
        result = FileAction::getFiles(dir,
            [postfix](fs::path p) {
                if (p.extension().string() == postfix)
                    return true;
                return false;
            });
        return result;
    }

}
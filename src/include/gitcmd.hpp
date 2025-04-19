#pragma once
#include "config.hpp"
#include "log.hpp"
#include "constant.hpp"
#include "robust_str.hpp"


#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace gitcmd
{
    inline void initRepo() {
        if (fs::exists(fs::path(Constant::defaultWebDir) / ".git")) {   //存在已经初始化的.git
            logger::warn << "git local repositories exists, please delete it then re init";
            return;
        }
        std::string cmd = "git init ";
        cmd += Constant::defaultWebDir;
        logger::warn << "excute: " << cmd << "\n";
        system(cmd.c_str());

        cmd = "git -C ";
        cmd += Constant::defaultWebDir;

        std::string pullcmd = cmd + " remote add origin " + config::config.repo;
        logger::warn << "excute: " << pullcmd << "\n";
        system(pullcmd.c_str());

        pullcmd = cmd + " pull origin " + config::config.branch + " --allow-unrelated-histories --allow-unrelated-histories";
        logger::warn << "excute: " << pullcmd << "\n";
        system(pullcmd.c_str());

        pullcmd = cmd + " add .";
        logger::warn << "excute: " << pullcmd << "\n";
        system(pullcmd.c_str());

        pullcmd = cmd + " commit -m \"Initial commit\"";
        logger::warn << "excute: " << pullcmd << "\n";
        system(pullcmd.c_str());

        pullcmd = cmd + " branch -m master main";
        logger::warn << "excute: " << pullcmd << "\n";
        system(pullcmd.c_str());

        pullcmd = cmd + " push origin " + config::config.branch + " --force";
        logger::warn << "excute: " << pullcmd << "\n";
        system(pullcmd.c_str());

    }

    inline void updateRepo() {
        std::string cmd = "git -C ";
        cmd += Constant::defaultWebDir;

        std::string pullcmd = cmd + " add .";
        logger::warn << "excute: " << pullcmd << "\n";
        system(pullcmd.c_str());

        pullcmd = cmd + " commit -m \"update commit-";
        auto now = std::chrono::system_clock::now();
        auto seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()
        );
        pullcmd += rstr::formatTimestamp(seconds_since_epoch);
        pullcmd += "\"";
        // logger::warn << "excute: " << pullcmd << "\n";
        system(pullcmd.c_str());

        pullcmd = cmd + " branch -m master main";
        // logger::warn << "excute: " << pullcmd << "\n";
        system(pullcmd.c_str());

        pullcmd = cmd + " push origin " + config::config.branch + " --force";
        // logger::warn << "excute: " << pullcmd << "\n";
        system(pullcmd.c_str());
    }
} // namespace gitcmd

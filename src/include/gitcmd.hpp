#pragma once
#include "config.hpp"
#include "log.hpp"
#include "constant.hpp"

namespace gitcmd
{
    inline void initRepo() {
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
} // namespace gitcmd

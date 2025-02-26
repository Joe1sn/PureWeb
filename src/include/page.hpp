/**
 * @file page.hpp
 * @author your name (you@domain.com)
 * @brief 生成各种页面的index.html
 * @version 0.1
 * @date 2025-02-23
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once
#include "config.hpp"
#include "actions.hpp"

#include <iostream>
#include <vector>

namespace Page
{
    std::vector<markdown::markdown> getAllMarkdownFiles();
} // namespace Page


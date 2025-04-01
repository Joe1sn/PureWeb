cmake_minimum_required(VERSION 3.5)
project(PureWeb LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.43.34808/bin/Hostx64/x64")

# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_INSTALL_PREFIX lib)

# different platform
if(WIN32)
    add_definitions(-DWINDOWS)
elseif(UNIX)
    add_definitions(-DLINUX)
endif()

add_subdirectory(extern/json)
add_subdirectory(extern/cmark)

include_directories(extern/json/include)
include_directories(extern/cmark/src)

set(PUREWEB_INCLUDE
    src/include/log.hpp
    src/include/constant.hpp
    src/include/config.hpp
    src/include/actions.hpp
    src/include/page.hpp
    src/include/markdown.hpp
    src/include/gitcmd.hpp
    src/include/html.hpp
)

set(PUREWEB_SOURCE
    src/main.cpp
    src/log.cpp
    src/config.cpp
    src/actions.cpp
    src/page.cpp
    src/markdown.cpp
    src/html.cpp
)

# WINDOWS 平台编译
if(MSVC)
    # Specify MSVC UTF-8 encoding
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

# find_package(jsoncpp REQUIRED)
add_executable(${PROJECT_NAME} ${PUREWEB_INCLUDE} ${PUREWEB_SOURCE})

# target_sources(${PROJECT_NAME} PRIVATE ${RESOURCES})
target_link_libraries(${PROJECT_NAME} cmark)

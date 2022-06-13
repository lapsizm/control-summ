cmake_minimum_required(VERSION 3.22.3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include("cmake/HunterGate.cmake")

HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.314.tar.gz"
        SHA1 "95c47c92f68edb091b5d6d18924baabe02a6962a"
)

project(ck)

hunter_add_package(Boost COMPONENTS program_options)
find_package(Boost CONFIG REQUIRED program_options)


add_executable(demo
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        )

target_link_libraries(demo Boost::program_options)

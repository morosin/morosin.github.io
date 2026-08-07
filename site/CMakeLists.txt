cmake_minimum_required(VERSION 3.25)
project(morosin_github_io CXX)

set(CMAKE_CXX_STANDARD 26)

add_executable(morosin_github_io_site morosin_github_io_site.cpp)
target_include_directories(morosin_github_io_site PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/htxx/include")
target_compile_options(morosin_github_io_site PUBLIC "-freflection")


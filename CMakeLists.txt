cmake_minimum_required(VERSION 3.29)
project(QLang)

set(CMAKE_CXX_STANDARD 23)

include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdParty)

file(
        GLOB_RECURSE sources
        LIST_DIRECTORIES true
        CONFIGURE_DEPENDS true
        "src/*.cpp"
)

add_executable(QLang ${sources})
cmake_minimum_required(VERSION 3.10)

project(lambda C)

file(GLOB SRC_FILES CONFIGURE_DEPENDS
    "src/*.c"
    "external/*.c"
)

add_executable(lambda ${SRC_FILES})

target_include_directories(lambda PRIVATE
    "external/"
)

target_link_libraries(lambda PRIVATE m readline)

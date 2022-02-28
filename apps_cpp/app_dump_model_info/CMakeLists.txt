cmake_minimum_required(VERSION 3.10.0)

project(app_dump_model_info)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/common.cmake)

set(APP_DUMP_MODEL_INFO_SRCS
    src/app_dump_model_info_main.cpp)

build_app(${PROJECT_NAME} APP_DUMP_MODEL_INFO_SRCS)

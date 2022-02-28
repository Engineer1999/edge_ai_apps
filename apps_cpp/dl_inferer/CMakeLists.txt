cmake_minimum_required(VERSION 3.10.0)

project(ti_dl_inferer)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/common.cmake)

set(DL_INFERER_SRCS
    src/ti_dl_inferer.cpp
    src/ti_dlr_inferer.cpp
    src/ti_onnx_inferer.cpp
    src/ti_tflite_inferer.cpp)

build_lib(${PROJECT_NAME} DL_INFERER_SRCS STATIC)

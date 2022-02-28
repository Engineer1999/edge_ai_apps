include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_compile_options(-std=c++17)

# Specific compile optios across all targets
#add_compile_definitions(MINIMAL_LOGGING)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()

# Turn off output data dumps for testing by default
OPTION(EDGEAI_ENABLE_OUTPUT_FOR_TEST "Enable Output Dumps for test" OFF)

# Check if we got an option from command line
if(EDGEAI_ENABLE_OUTPUT_FOR_TEST)
    message("EDGEAI_ENABLE_OUTPUT_FOR_TEST enabled")
    add_definitions(-DEDGEAI_ENABLE_OUTPUT_FOR_TEST)
endif()

message(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE} PROJECT_NAME = ${PROJECT_NAME}")

SET(CMAKE_FIND_LIBRARY_PREFIXES "" "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".lib" ".so")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})

set(TARGET_PLATFORM     J7)
set(TARGET_CPU          A72)
set(TARGET_OS           LINUX)

set(TENSORFLOW_INSTALL_DIR /opt/tensorflow)
set(ONNXRT_INSTALL_DIR /opt/onnxruntime)
set(DLPACK_INSTALL_DIR /opt/dlpack)

add_definitions(
    -DTARGET_CPU=${TARGET_CPU}
    -DTARGET_OS=${TARGET_OS}
)

link_directories(/usr/local/dlr
                 /usr/lib/aarch64-linux-gnu
                 /usr/lib/
                 ${TENSORFLOW_INSTALL_DIR}/tensorflow/lite/tools/make/gen/linux_aarch64/lib
                 )

#message("PROJECT_SOURCE_DIR =" ${PROJECT_SOURCE_DIR})
#message("CMAKE_SOURCE_DIR =" ${CMAKE_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/include
                    /usr/local/include
                    /usr/local/dlr
                    /usr/include/gstreamer-1.0/
                    /usr/include/glib-2.0/
                    /usr/lib/aarch64-linux-gnu/glib-2.0/include
                    /usr/include/opencv4/
                    /usr/include/processor_sdk/vision_apps/
                    ${TENSORFLOW_INSTALL_DIR}
                    ${TENSORFLOW_INSTALL_DIR}/tensorflow/lite/tools/make/downloads/flatbuffers/include
                    ${ONNXRT_INSTALL_DIR}/include/onnxruntime
                    ${ONNXRT_INSTALL_DIR}/include/onnxruntime/core/session
                    ${DLPACK_INSTALL_DIR}/include
                    )

set(COMMON_LINK_LIBS
    edgeai_utils
    edgeai_common
    ti_dl_inferer
    )

set(SYSTEM_LINK_LIBS
    ncurses
    tinfo
    gstreamer-1.0
    glib-2.0
    gobject-2.0
    gstapp-1.0
    opencv_core
    opencv_imgproc
    yaml-cpp
    dlr
    tensorflow-lite
    onnxruntime
    pthread
    dl
    tivision_apps
    )

# Function for building a node:
# ARG0: app name
# ARG1: source list
function(build_app)
    set(app ${ARGV0})
    set(src ${ARGV1})
    add_executable(${app} ${${src}})
    target_link_libraries(${app}
                          -Wl,--start-group
                          ${COMMON_LINK_LIBS}
                          ${TARGET_LINK_LIBS}
                          ${SYSTEM_LINK_LIBS}
                          -Wl,--end-group)
endfunction(build_app)

# Function for building a node:
# ARG0: lib name
# ARG1: source list
# ARG2: type (STATIC, SHARED)
function(build_lib)
    set(lib ${ARGV0})
    set(src ${ARGV1})
    set(type ${ARGV2})
    set(version 1.0.0)

    add_library(${lib} ${type} ${${src}})

    get_filename_component(PROJ_DIR "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

    set(INC_DIR_DST ${CMAKE_INSTALL_LIBDIR}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJ_DIR})

    install(TARGETS ${lib}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # Shared Libs
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}  # Static Libs
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # Executables, DLLs
            INCLUDES DESTINATION ${INC_DIR_DST}
    )

    # Specify the header files to install
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
        DESTINATION ${INC_DIR_DST}
    )

endfunction(build_lib)


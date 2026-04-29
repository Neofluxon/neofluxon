find_package(OpenMP REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(JPEG_TURBO REQUIRED libturbojpeg)
find_path(STB_INCLUDE_DIR stb_image_resize2.h PATH_SUFFIXES stb)

file(GLOB CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_library(NfCore STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

add_library(${PROJECT_NAME}::CoreLibrary ALIAS NfCore)

target_include_directories(NfCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${JPEG_TURBO_INCLUDE_DIRS}
    ${JPEG_TURBO_INCLUDE_DIRS}
)

target_link_libraries(NfCore PRIVATE
        raw
        OpenMP::OpenMP_CXX
        ${JPEG_TURBO_LIBRARIES}
)

find_package(OpenMP REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

pkg_check_modules(JPEG_TURBO REQUIRED libturbojpeg)
find_path(STB_INCLUDE_DIR stb_image_resize2.h PATH_SUFFIXES stb)

if(STB_INCLUDE_DIR)
    message(STATUS "Found stb_image_resize2.h at: ${STB_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "stb_image_resize2.h not found! Install libstb-dev.")
endif()

set(CORE_SOURCES
        NeofluxonCore.cpp
        NfCache.cpp
        NfForegroundThreadPool.cpp
        NfImage.cpp
        NfImageData.cpp
        NfImageDecoder.cpp
        NfImageDecoderFactory.cpp
        NfImageTask.cpp
        NfJpegImageDecoder.cpp
        NfLogger.cpp
        NfPathScanner.cpp
        NfPhoto.cpp
        NfPhotoId.cpp
        NfPhotoLoader.cpp
        NfPngImageDecoder.cpp
        NfPreview.cpp
        NfPreviewTask.cpp
        NfRawImageDecoder.cpp
        NfScheduler.cpp
        NfTask.cpp
        NfThumbnail.cpp
        NfThumbnailTask.cpp
        library/NfLibraryDatabase.cpp)

set(CORE_HEADERS
        NeofluxonCore.h
        Neofluxon.h
        NfCache.h
        NfForegroundThreadPool.h
        NfImageData.h
        NfImageDecoderFactory.h
        NfImageDecoder.h
        NfImage.h
        NfImageTask.h
        NfJpegImageDecoder.h
        NfLogger.h
        NfPathScanner.h
        NfPhoto.h
        NfPhotoId.h
        NfPhotoLoader.h
        NfPngImageDecoder.h
        NfPreview.h
        NfPreviewTask.h
        NfRawImageDecoder.h
        NfScheduler.h
        NfTask.h
        NfThumbnail.h
        NfThumbnailTask.h
        library/NfLibraryDatabase.h)

add_library(NfCore STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

add_library(${PROJECT_NAME}::CoreLibrary ALIAS NfCore)

target_include_directories(NfCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${JPEG_TURBO_INCLUDE_DIRS}
    ${STB_INCLUDE_DIR}
    ${PNG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)

target_link_libraries(NfCore PRIVATE
        raw
        OpenMP::OpenMP_CXX
        ${JPEG_TURBO_LIBRARIES}
        ${PNG_LIBRARIES}
        ${ZLIB_LIBRARIES}
)

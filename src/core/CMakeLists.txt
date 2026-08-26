#find_package(OpenMP REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(SQLite3 REQUIRED)

pkg_check_modules(JPEG_TURBO REQUIRED libturbojpeg)
find_path(STB_INCLUDE_DIR stb_image_resize2.h PATH_SUFFIXES stb)

if(STB_INCLUDE_DIR)
    message(STATUS "Found stb_image_resize2.h at: ${STB_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "stb_image_resize2.h not found! Install libstb-dev.")
endif()

set(CORE_HEADERS
        utils/NfTimeUtils.h
        NfThreadAllocationManager.h
        NeofluxonCore.h
        Neofluxon.h
        NfCache.h
        NfThreadPool.h
        NfImageData.h
        NfImageDecoderFactory.h
        NfImageDecoder.h
        NfImage.h
        NfImageTask.h
        NfJpegImageDecoder.h
        NfLogger.h
        NfPhotoSource.h
        NfFilesystemPhotoSource.h
        NfFilesystemPhotoScanTask.h
        NfPhotoScanner.h
        NfPhotoId.h
        NfPhoto.h
        NfPhotoSummary.h
        NfPhotoMetadataExtractor.h
        NfDirectoryIterator.h
        NfPhotoDirectoryIterator.h
        NfPhotoLoader.h
        NfPngImageDecoder.h
        NfPreview.h
        NfPreviewTask.h
        NfRawImageDecoder.h
        NfScheduler.h
        NfTask.h
        NfThumbnail.h
        NfThumbnailTask.h
        library/NfLibrary.h
        library/NfLibraryDatabase.h
        library/NfLibraryFolderImportTask.h
        library/NfSourceRecords.h
        library/NfLibraryPhotoSource.h
        library/NfLibraryPhotoScanTask.h
        library/NfLibraryManager.h
        library/NfLibraryRepresentation.h
        library/NfLibraryTreeNode.h
        )

set(CORE_SOURCES
        utils/NfTimeUtils.cpp
        NfThreadAllocationManager.cpp
        NeofluxonCore.cpp
        NfCache.cpp
        NfThreadPool.cpp
        NfImage.cpp
        NfImageData.cpp
        NfImageDecoder.cpp
        NfImageDecoderFactory.cpp
        NfImageTask.cpp
        NfJpegImageDecoder.cpp
        NfLogger.cpp
        NfFilesystemPhotoSource.cpp
        NfFilesystemPhotoScanTask.cpp
        NfPhotoScanner.cpp
        NfPhotoId.cpp
        NfPhoto.cpp
        NfPhotoMetadataExtractor.cpp
        NfDirectoryIterator.cpp
        NfPhotoDirectoryIterator.cpp
        NfPhotoLoader.cpp
        NfPngImageDecoder.cpp
        NfPreview.cpp
        NfPreviewTask.cpp
        NfRawImageDecoder.cpp
        NfScheduler.cpp
        NfTask.cpp
        NfThumbnail.cpp
        NfThumbnailTask.cpp
        library/NfLibrary.cpp
        library/NfLibraryDatabase.cpp
        library/NfLibraryFolderImportTask.cpp
        library/NfLibraryPhotoSource.cpp
        library/NfLibraryPhotoScanTask.cpp
        library/NfLibraryManager.cpp
        library/NfLibraryRepresentation.cpp
        library/NfLibraryTreeNode.cpp
        )

add_library(NfCore STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

add_library(${PROJECT_NAME}::CoreLibrary ALIAS NfCore)

target_include_directories(NfCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/library
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${JPEG_TURBO_INCLUDE_DIRS}
    ${STB_INCLUDE_DIR}
    ${PNG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(NfCore PRIVATE
        raw
#        OpenMP::OpenMP_CXX
        ${JPEG_TURBO_LIBRARIES}
        ${PNG_LIBRARIES}
        ${ZLIB_LIBRARIES}
        SQLite::SQLite3
)

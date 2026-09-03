# --- Sources Definition ---
set(MODEL_HEADERS
    NfBrowserModel.h
    NfFolderModel.h
    NfLibraryModel.h
    NfLibraryTreeItem.h
    NfLibraryTreeModel.h
    NfMetadataFilterProxy.h
)

set(MODEL_SOURCES
    NfBrowserModel.cpp
    NfFolderModel.cpp
    NfLibraryModel.cpp
    NfLibraryTreeItem.cpp
    NfLibraryTreeModel.cpp
    NfMetadataFilterProxy.cpp
)

add_library(NfDesktopModels STATIC
    ${MODEL_SOURCES}
    ${MODEL_HEADERS}
)

add_library(${PROJECT_NAME}::DesktopModels ALIAS NfDesktopModels)

target_include_directories(NfDesktopModels PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(NfDesktopModels
        PUBLIC
        Qt6::Core
        Qt6::Gui
        ${PROJECT_NAME}::CommonUiLibrary
)

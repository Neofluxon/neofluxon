set(WIDGETS_HEADERS
    NfStyledWidget.h
    NfThumbnailsView.h
    NfCentralWidget.h
    NfLeftPanel.h
    NfMainMenu.h
    NfPanel.h
    NfFolderActionDelegate.h
    NfPathBrowser.h
    NfLibraryTreeView.h
    NfSettingsDialog.h
    NfBrowserView.h
    NfFolderView.h
    NfLibraryView.h
    NfPhotoPreviewView.h
    NfTopBar.h
)

set(WIDGETS_SOURCES
    NfStyledWidget.cpp
    NfThumbnailsView.cpp
    NfCentralWidget.cpp
    NfLeftPanel.cpp
    NfMainMenu.cpp
    NfPanel.cpp
    NfFolderActionDelegate.cpp
    NfPathBrowser.cpp
    NfLibraryTreeView.cpp
    NfSettingsDialog.cpp
    NfBrowserView.cpp
    NfFolderView.cpp
    NfLibraryView.cpp
    NfPhotoPreviewView.cpp
    NfTopBar.cpp
)

add_library(NfDesktopWidgets STATIC
    ${WIDGETS_SOURCES}
    ${WIDGETS_HEADERS}
)

add_library(${PROJECT_NAME}::DesktopWidgets ALIAS NfDesktopWidgets)

target_include_directories(NfDesktopWidgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Dependencies
target_link_libraries(NfDesktopWidgets PUBLIC
        Qt6::Widgets
        ${PROJECT_NAME}::CommonUiLibrary
        ${PROJECT_NAME}::DesktopModels
)

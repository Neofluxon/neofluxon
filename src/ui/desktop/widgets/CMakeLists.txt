set(WIDGETS_HEADERS
    NfStyledWidget.h
    NfScrollStack.h
    NfCollapsibleSection.h
    NfThumbnailDelegate.h
    NfThumbnailsView.h
    NfCentralWidget.h
    NfPanel.h
    NfLeftPanel.h
    NfRightPanel.h
    NfMainMenu.h
    NfFolderActionDelegate.h
    NfPathBrowser.h
    NfLibraryListHeader.h
    NfLibraryListView.h
    NfRepresentationListView.h
    NfLibraryTreeView.h
    NfLibraryBrowser.h
    NfSettingsDialog.h
    NfBrowserView.h
    NfFolderView.h
    NfLibraryView.h
    NfNewLibraryDialog.h
    NfPhotoPreviewView.h
    NfTopBar.h
    NfBrowserTopBar.h
    NfBrowserSortBar.h
    NfBrowserFilterBar.h
    NfBrowserViewModeBar.h
    # NfBreadcrumbView.h
    NfImageMetadataPanel.h
    NfMetadataSectionWidget.h
)

set(WIDGETS_SOURCES
    NfStyledWidget.cpp
    NfScrollStack.cpp
    NfCollapsibleSection.cpp
    NfThumbnailDelegate.cpp
    NfThumbnailsView.cpp
    NfCentralWidget.cpp
    NfPanel.cpp
    NfLeftPanel.cpp
    NfRightPanel.cpp
    NfMainMenu.cpp
    NfFolderActionDelegate.cpp
    NfPathBrowser.cpp
    NfLibraryListHeader.cpp
    NfLibraryListView.cpp
    NfRepresentationListView.cpp
    NfLibraryTreeView.cpp
    NfLibraryBrowser.cpp
    NfSettingsDialog.cpp
    NfBrowserView.cpp
    NfFolderView.cpp
    NfLibraryView.cpp
    NfNewLibraryDialog.cpp
    NfPhotoPreviewView.cpp
    NfTopBar.cpp
    NfBrowserTopBar.cpp
    NfBrowserSortBar.cpp
    NfBrowserFilterBar.cpp
    NfBrowserViewModeBar.cpp
 #   NfBreadcrumbView.cpp
    NfImageMetadataPanel.cpp
    NfMetadataSectionWidget.cpp
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

set(COMMON_HEADERS
    NfQPixmap.h
    NfQtPathUtils.h
    context/NfContext.h
    state/NfUiBrowserState.h
    state/NfUiFolderModeState.h
    state/NfUiLibraryModeState.h
    state/NfUiShootsModeState.h
    state/NfUiState.h
    NfPhotoProvider.h
    NfLibraryAdapter.h
    NfUserTasksAdapter.h
    NfMetadataProvider.h
)

set(COMMON_SOURCES
    NfQPixmap.cpp
    NfQtPathUtils.cpp
    state/NfUiBrowserState.cpp
    state/NfUiFolderModeState.cpp
    state/NfUiLibraryModeState.cpp
    state/NfUiShootsModeState.cpp
    state/NfUiState.cpp
    NfPhotoProvider.cpp
    NfLibraryAdapter.cpp
    NfUserTasksAdapter.cpp
    NfMetadataProvider.cpp
)

add_library(NfCommonUi STATIC
    ${COMMON_SOURCES}
    ${COMMON_HEADERS}
)

add_library(${PROJECT_NAME}::CommonUiLibrary ALIAS NfCommonUi)

target_include_directories(NfCommonUi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/context
    ${CMAKE_CURRENT_SOURCE_DIR}/state
)

target_link_libraries(NfCommonUi
        PUBLIC
        Qt6::Core
        Qt6::Gui
        ${PROJECT_NAME}::CoreLibrary
)

if(WIN32)
    target_link_libraries(NfCommonUi PRIVATE
        shell32
        ole32
    )
endif()

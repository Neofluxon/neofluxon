set(DESKTOP_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/NfApplication.h
   ${CMAKE_CURRENT_SOURCE_DIR}/NfMainWindow.h
)

set(DESKTOP_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/NfApplication.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/NfMainWindow.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

# Create executable
add_executable(${PROJECT_NAME}
    ${DESKTOP_SOURCES}
    ${DESKTOP_HEADERS}
    resources/resources.qrc
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    ${PROJECT_NAME}::CoreLibrary
    ${PROJECT_NAME}::CommonUiLibrary
    ${PROJECT_NAME}::DesktopModels
    ${PROJECT_NAME}::DesktopWidgets
)

# Include directories for UI-only headers
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set output directories
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(FILES
    "${CMAKE_SOURCE_DIR}/packaging/desktop/neofluxon.desktop"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
)

foreach(icon_size 16 24 32 48 64 128 256 512)
    install(FILES
        "${CMAKE_SOURCE_DIR}/packaging/icons/${icon_size}x${icon_size}/neofluxon.png"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/${icon_size}x${icon_size}/apps"
    )
endforeach()

install(FILES
    "${CMAKE_SOURCE_DIR}/packaging/icons/neofluxon.svg"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
)

add_subdirectory(models)
add_subdirectory(widgets)

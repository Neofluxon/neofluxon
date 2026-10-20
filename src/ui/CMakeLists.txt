set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(UI_RESOURCES resources/resources.qrc)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(Qt6 REQUIRED COMPONENTS DBus)
endif()

add_subdirectory(common)
add_subdirectory(desktop)

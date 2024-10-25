cmake_minimum_required(VERSION 3.28.3)
project(SlippyMap VERSION 1.0.1 DESCRIPTION "SlippyMap Tiled Map Widget Library")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

add_subdirectory(examples)

export(PACKAGE SlippyMap)

add_library(SlippyMap
        include/SlippyMap/SlippyMap.h
        include/SlippyMap/SlippyMapLayer.h
        include/SlippyMap/SlippyMapLayerManager.h
        include/SlippyMap/SlippyMapLayerMarker.h
        include/SlippyMap/SlippyMapLayerObject.h
        include/SlippyMap/SlippyMapWidget.h
        include/SlippyMap/SlippyMapWidgetLayer.h
        src/SlippyMap.cpp
        src/SlippyMapLayer.cpp
        src/SlippyMapLayerManager.cpp
        src/SlippyMapLayerMarker.cpp
        src/SlippyMapLayerObject.cpp
        src/SlippyMapWidget.cpp
        src/SlippyMapWidgetLayer.cpp
)

target_include_directories(SlippyMap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(SlippyMap Qt5::Widgets)
target_link_libraries(SlippyMap Qt5::Network)

set_target_properties(SlippyMap PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(SlippyMap PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

set(SLIPPYMAP_PUBLIC_HEADERS
        include/SlippyMap/SlippyMap.h
        include/SlippyMap/SlippyMapLayer.h
        include/SlippyMap/SlippyMapLayerManager.h
        include/SlippyMap/SlippyMapLayerMarker.h
        include/SlippyMap/SlippyMapLayerObject.h
        include/SlippyMap/SlippyMapWidget.h
        include/SlippyMap/SlippyMapWidgetLayer.h
)

set_target_properties(SlippyMap PROPERTIES PUBLIC_HEADER "${SLIPPYMAP_PUBLIC_HEADERS}")

include(GNUInstallDirs)

install(TARGETS SlippyMap
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SlippyMap
)

configure_file(SlippyMap.pc.in SlippyMap.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/SlippyMap.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

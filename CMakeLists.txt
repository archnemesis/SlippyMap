cmake_minimum_required(VERSION 3.28.3)
project(SlippyMap VERSION 1.0.1 DESCRIPTION "SlippyMap Tiled Map Widget Library")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0153 OLD)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

add_subdirectory(examples)
add_subdirectory(externals/Qt-Color-Widgets)

export(PACKAGE SlippyMap)

add_library(SlippyMap
        include/SlippyMap/SlippyMap.h
        include/SlippyMap/SlippyMapLayer.h
        include/SlippyMap/SlippyMapLayerManager.h
        include/SlippyMap/SlippyMapWidgetMarker.h
        include/SlippyMap/SlippyMapLayerObject.h
        include/SlippyMap/SlippyMapWidget.h
        include/SlippyMap/SlippyMapWidgetLayer.h
        include/SlippyMap/SlippyMapLayerPolygon.h
        include/SlippyMap/SlippyMapLayerMarkerPropertyPage.h
        include/SlippyMap/SlippyMapLayerObjectPropertyPage.h
        include/SlippyMap/SlippyMapLayerPolygonPropertyPage.h
        src/SlippyMap.cpp
        src/SlippyMapLayer.cpp
        src/SlippyMapLayerManager.cpp
        src/SlippyMapWidgetMarker.cpp
        src/SlippyMapLayerObject.cpp
        src/SlippyMapWidget.cpp
        src/SlippyMapWidgetLayer.cpp
        src/SlippyMapLayerPolygon.cpp
        src/SlippyMapLayerMarkerPropertyPage.cpp
        src/SlippyMapLayerObjectPropertyPage.cpp
        src/SlippyMapLayerPolygonPropertyPage.cpp
        include/SlippyMap/SlippyMapLayerObjectCommonPropertyPage.h
        src/SlippyMapLayerObjectCommonPropertyPage.cpp
        include/SlippyMap/SlippyMapLayerEllipse.h
        src/SlippyMapLayerEllipse.cpp
        include/SlippyMap/SlippyMapAnimatedLayer.h
        src/SlippyMapAnimatedLayer.cpp
        include/SlippyMap/SlippyMapLayerPath.h
        src/SlippyMapLayerPath.cpp
)

target_include_directories(SlippyMap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(SlippyMap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/externals/Qt-Color-Widgets/include)
target_include_directories(SlippyMap PRIVATE "D:/Libraries/boost_1_86_0")

target_link_libraries(SlippyMap PRIVATE Qt5::Widgets)
target_link_libraries(SlippyMap PRIVATE Qt5::Network)
target_link_libraries(SlippyMap PRIVATE QtColorWidgets)

set_target_properties(SlippyMap PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(SlippyMap PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

set(SLIPPYMAP_PUBLIC_HEADERS
        include/SlippyMap/SlippyMap.h
        include/SlippyMap/SlippyMapLayer.h
        include/SlippyMap/SlippyMapLayerManager.h
        include/SlippyMap/SlippyMapWidgetMarker.h
        include/SlippyMap/SlippyMapLayerObject.h
        include/SlippyMap/SlippyMapWidget.h
        include/SlippyMap/SlippyMapWidgetLayer.h
        include/SlippyMap/SlippyMapLayerPolygon.h
)

set_target_properties(SlippyMap PROPERTIES PUBLIC_HEADER "${SLIPPYMAP_PUBLIC_HEADERS}")

include(GNUInstallDirs)

install(TARGETS SlippyMap
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SlippyMap
)

configure_file(SlippyMap.pc.in SlippyMap.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SlippyMap.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

project(simple-map)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)

add_executable(simple-map simple-map.cpp)
target_include_directories(simple-map PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(simple-map Qt5::Widgets)
target_link_libraries(simple-map Qt5::Network)
target_link_libraries(simple-map SlippyMap)

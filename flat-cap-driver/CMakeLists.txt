project(indi-flat-cap-driver C CXX)
cmake_minimum_required(VERSION 2.8.12)

include(GNUInstallDirs)

# add our cmake_modules directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")

find_package(INDI REQUIRED)
find_package(Nova REQUIRED)
find_package(ZLIB REQUIRED)
find_package(GSL REQUIRED)

# set version in config.h and our driver's xml file
set(CDRIVER_VERSION_MAJOR 0)
set(CDRIVER_VERSION_MINOR 1)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/indi_flatcapdriver.xml.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/indi_flatcapdriver.xml
)

# set our include directories
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENt_SOURCE_DIR})
include_directories(${INDI_INCLUDE_DIR})
include_directories(${NOVA_INCLUDE_DIR})
include_directories(${EV_INCLUDE_DIR})

include(CMakeCommon)

# tell cmake to build our executable
add_executable(
    indi_flatcapdriver
    indi_flatcapdriver.cpp
)

# and link it
target_link_libraries(
    indi_flatcapdriver
    ${INDI_LIBRARIES}
    ${NOVA_LIBRARIES}
    ${GSL_LIBRARIES}
)

# tell cmake where to install the executable
install(TARGETS indi_flatcapdriver RUNTIME DESTINATION bin)

# and where to put the driver's xml file
install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/indi_flatcapdriver.xml
    DESTINATION ${INDI_DATA_DIR}
)

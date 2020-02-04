cmake_minimum_required(VERSION 3.1.0)

project(peony-qt-share VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(PROJECTNAME peony-qt-admin)

find_package(Qt5 COMPONENTS Core Gui Widgets Concurrent REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(PEONY REQUIRED peony)
message(${PEONY_INCLUDE_DIRS})
message(${PEONY_LIBRARIES})

include_directories(${PEONY_INCLUDE_DIRS})

aux_source_directory(. SRC_FILES)

file(GLOB_RECURSE QRC_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.qrc)
set(QRC_SOURCE_FILES resources.qrc)
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})

add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${QRC_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent ${PEONY_LIBRARIES})

install(TARGETS peony-qt-share DESTINATION /usr/lib/peony-qt-extensions)

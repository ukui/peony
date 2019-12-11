cmake_minimum_required(VERSION 3.0.0)

project(peony-qt-extensions VERSION 0.1 LANGUAGES CXX)

include(ExternalProject)

set(peony-qt-extensions-cmake-impl-dir ${PROJECT_SOURCE_DIR}/peony-qt-extensions-cmake)
set(peony-qt-extensions-qmake-impl-dir ${PROJECT_SOURCE_DIR}/peony-qt-extensions-qmake)

ExternalProject_Add(peony-qt-extensions-cmake
  SOURCE_DIR        ${peony-qt-extensions-cmake-impl-dir}
  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/peony-qt-extensions-cmake
  BUILD_IN_SOURCE   1
  CONFIGURE_COMMAND cmake ${peony-qt-extensions-cmake-impl-dir}
  BUILD_COMMAND     make
  EXCLUDE_FROM_ALL TRUE
  )

ExternalProject_Add(peony-qt-extensions-qmake
  SOURCE_DIR        ${peony-qt-extensions-qmake-impl-dir}
  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/peony-qt-extensions-qmake
  BUILD_IN_SOURCE   1
  CONFIGURE_COMMAND qmake ${peony-qt-extensions-qmake-impl-dir}
  BUILD_COMMAND     make
  EXCLUDE_FROM_ALL TRUE
  )

cmake_minimum_required(VERSION 3.0.0)

project(peony-extensions VERSION 0.1 LANGUAGES CXX)

include(ExternalProject)

set(peony-extensions-cmake-impl-dir ${PROJECT_SOURCE_DIR}/peony-extensions-cmake)
set(peony-extensions-qmake-impl-dir ${PROJECT_SOURCE_DIR}/peony-extensions-qmake)

ExternalProject_Add(peony-extensions-cmake
  SOURCE_DIR        ${peony-extensions-cmake-impl-dir}
  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/peony-extensions-cmake
  BUILD_IN_SOURCE   1
  CONFIGURE_COMMAND cmake ${peony-extensions-cmake-impl-dir}
  BUILD_COMMAND     make
  INSTALL_COMMAND   cmake -E echo "skip install step."
  )

ExternalProject_Add(peony-extensions-qmake
  SOURCE_DIR        ${peony-extensions-qmake-impl-dir}
  PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/peony-extensions-qmake
  BUILD_IN_SOURCE   1
  CONFIGURE_COMMAND qmake ${peony-extensions-qmake-impl-dir}
  BUILD_COMMAND     make
  INSTALL_COMMAND   cmake -E echo "skip install step."
  )

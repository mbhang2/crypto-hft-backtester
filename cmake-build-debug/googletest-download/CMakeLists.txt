cmake_minimum_required(VERSION 3.5.1)

project(googletest-download NONE)

include(ExternalProject)
ExternalProject_Add(googletest
  GIT_REPOSITORY    https://github.com/google/googletest.git
  GIT_TAG           v1.14.0
  SOURCE_DIR        "/Users/zexuanliu/CLionProjects/group_04_project/cmake-build-debug/googletest-src"
  BINARY_DIR        "/Users/zexuanliu/CLionProjects/group_04_project/cmake-build-debug/googletest-build"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)

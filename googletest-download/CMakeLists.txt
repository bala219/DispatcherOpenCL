cmake_minimum_required(VERSION 2.8.2)

project(googletest-download NONE)

include(ExternalProject)
ExternalProject_Add(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG f1a87d73fc604c5ab8fbb0cc6fa9a86ffd845530
    SOURCE_DIR "/home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms/googletest-src"
    BINARY_DIR "/home/gurumurt/Phd/HeroDB/SQL_flow/db-engine-paradigms/googletest-build"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND ""
)

cmake_minimum_required(VERSION 2.8.9)
add_subdirectory(lib)
add_subdirectory(example)

install(DIRECTORY plugin/ DESTINATION ${CMAKE_INSTALL_PREFIX}/plugins/MLP)

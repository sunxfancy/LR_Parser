cmake_minimum_required(VERSION 2.8)
project(lr_parser)
SET(CMAKE_CXX_COMPILER_ENV_VAR "CXX")
SET(CMAKE_CXX_FLAGS "-std=c++11")
include_directories(includes lib/lex_include lib/lua_include)
file(GLOB_RECURSE lr_source_files "${CMAKE_CURRENT_SOURCE_DIR}/sources/*.cpp")
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_executable(lr_parser ${lr_source_files})
install(TARGETS lr_parser RUNTIME DESTINATION bin)
TARGET_LINK_LIBRARIES(lr_parser lex lua dl)


FIND_PACKAGE(Doxygen)  
OPTION(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})  
  
IF(BUILD_DOCUMENTATION)  
    IF(NOT DOXYGEN_FOUND)  
        MESSAGE(FATAL_ERROR "Doxygen is needed to build the documentation.")  
    ENDIF()  
  
    SET(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)  
    SET(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)  
  
    CONFIGURE_FILE(${doxyfile_in} ${doxyfile} @ONLY)  
  
    ADD_CUSTOM_TARGET(doc  
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}  
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}  
        COMMENT "Generating API documentation with Doxygen"  
        VERBATIM)  
  
    INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)  
ENDIF()  

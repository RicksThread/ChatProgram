cmake_minimum_required(VERSION 3.10)

#create the variable program_name and set it to the value of the executable's name
set(PROGRAMFILE_NAME RLY_client)
set(PROJECT_NAME RLY_client)
# here we specify that the project is C language only, so the default
# C compiler on the system will be used
project(${PROJECT_NAME} C)

set(EXTERNAL_LIBS_NAME externals)

string(APPEND CMAKE_EXE_LINKER_FLAGS "")
string(APPEND CMAKE_C_FLAGS "-Wall")

#find all the src files for the utils
file(GLOB_RECURSE UTILS_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../${EXTERNAL_LIBS_NAME}/utils/*.c)
#file(GLOB_RECURSE STDRELY_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../${EXTERNAL_LIBS_NAME}/stdrely/*.c)

#create the library archive (.a) through the source files
add_library(utils STATIC ${UTILS_SRCS})
#add_library(stdrely STATIC ${STDRELY_SRCS})

file(GLOB_RECURSE SRC_FILES src/*.c)

add_executable(${PROGRAMFILE_NAME} ${SRC_FILES})

#link the library to the program
target_link_libraries(${PROGRAMFILE_NAME} PUBLIC utils)

#include the referenced directories
target_include_directories(${PROGRAMFILE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#include external headers
target_include_directories(${PROGRAMFILE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../${EXTERNAL_LIBS_NAME}/utils)
target_include_directories(${PROGRAMFILE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../${EXTERNAL_LIBS_NAME}/stdrely)
target_include_directories(${PROGRAMFILE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../${EXTERNAL_LIBS_NAME}/uthash_lib/src)
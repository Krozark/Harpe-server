cmake_minimum_required (VERSION 2.8)

# define a macro that helps defining an option
macro(set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

# project name
project (harpe-server)

# setup version numbers
set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)
set(VERSION_PATCH 7)


# add the  header path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# add an option for choosing the build type (shared or static)
#set_option(BUILD_SHARED_LIBS FALSE BOOL "TRUE to build as shared libraries, FALSE to build it as static libraries")

# add an option for building the API documentation
set_option(BUILD_DOC TRUE BOOL "TRUE to generate the API documentation, FALSE to ignore it")


#set(includepath "-I${CMAKE_CURRENT_SOURCE_DIR}/include")
set(DEFINES "")

set(LIBS "-lpthread")
if(${WIN32})
    find_library(PTHREAD_LIBS "winpthread-1")
    find_library(SOCKET_LIBS ws2_32)
    find_library(KERNEL_LIBS kernel32)
    set(LIBS ${PTHREAD_LIBS} ${SOCKET_LIBS} ${KERNEL_LIBS})
else()
    find_library(PTHREAD_LIBS pthread)
    set(LIBS ${PTHREAD_LIBS})
endif()

find_library(MYSQL_LIB mysqlcppconn)
find_library(SQLITE_LIB sqlite3)
find_library(ORM_LIB orm)
find_library(MGF_LIB mgf)
find_library(SOCKET_LIB socket)
find_library(UTILS_LIB utils)
set(LIBS ${LIBS} ${MYSQL_LIB} ${SQLITE_LIB} ${ORM_LIB} ${MGF_LIB} ${SOCKET_LIB} ${UTILS_LIB})

set(WARNING "-Wall")
#set(FLAGS "-o3 -std=c++0x -Wl,--no-as-needed -ldl")
set(FLAGS "-o3 -std=c++0x")
set(CMAKE_CXX_FLAGS "${DEFINES} ${WARNING} ${FLAGS}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(LIB_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} )

# add the subdirectories
add_subdirectory(src/${CMAKE_PROJECT_NAME})

#build doc
if(BUILD_DOC)
    add_subdirectory(doc)
endif()


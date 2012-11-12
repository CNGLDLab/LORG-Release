project(Lorg)
cmake_minimum_required(VERSION 2.8)

# The version number.
set (Lorg_VERSION_MAJOR 1)
set (Lorg_VERSION_MINOR 0)


# Boost nécessaire
find_package(Boost 1.46 COMPONENTS regex program_options system filesystem thread unit_test_framework REQUIRED)
link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )
# librt nécessaire sous Linux pour Boost
if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    set(RTLIB rt)
endif()


# multithreading
option(MULTITHREAD "Enable multithreading where available" on)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/")
find_package(TBB)
set(TB2GRAM 0)
if (MULTITHREAD)
    if (DEFINED TBB_FOUND)
        add_definitions(-DUSE_THREADS=1)
        set(TBB_LIBS ${TBB_LIBRARIES})
        set(WITH_TB2GRAM 1)
    else()
        message(FATAL_ERROR "tbb libraries not found on your system. multithreaded version unavailable")
    endif()
else()
    if (DEFINED TBB_FOUND)
        message(WARNING "As the MULTITHREAD option is not set, the tb2gram executable will not be produced (tb2gram is the grammar training tool).")
    else()
        message(WARNING "Install TBB and enable the MULTITHREAD option if you want to compile tb2gram (the grammar training tool)")
    endif()
endif()
    

# tcmalloc
option(WANT_TCMALLOC "Require use of tcmalloc" off)
if(WANT_TCMALLOC)
    find_package(GooglePerfTools COMPONENTS tcmalloc REQUIRED)
    if (NOT GOOGLE_PERFTOOLS_FOUND)
        message(FATAL_ERROR "tcmalloc not found on your system")
    endif()
    set(TCMALLOC_LIB ${TCMALLOC_LIBRARY})
endif()


# targets are in src and src/training subdirectories
add_subdirectory(src)
add_subdirectory(test)


# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LORG_License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "${Lorg_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${Lorg_VERSION_MINOR}")
# four \ needed to escape special meaning of .
set(CPACK_SOURCE_IGNORE_FILES
     "~$"
     ".*/\\\\."
     "^${PROJECT_SOURCE_DIR}/install"
     "^${PROJECT_SOURCE_DIR}/test"
     "^${PROJECT_SOURCE_DIR}/build")
include (CPack)

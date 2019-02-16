cmake_minimum_required (VERSION 3.5)

project (FLOWER)


###############################################################################
#####  Boilerplate Setup
###############################################################################
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
message(STATUS "** Project will be installed to ${CMAKE_INSTALL_PREFIX}")


set(CMAKE_VERBOSE_MAKEFILE OFF)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
include(BuildType)
include(IdentifyOS)


option(ENABLE_TESTS "Enable tests" ON)


###############################################################################
#####  Offer the builder the choice of overriding the installation directories
###############################################################################
set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKEDIR CMake)
else()
  set(DEF_INSTALL_CMAKEDIR share/cmake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKEDIR ${DEF_INSTALL_CMAKEDIR} CACHE PATH "Installation directory for CMake files")


###############################################################################
##### Report to builder
###############################################################################
foreach(p LIB BIN INCLUDE CMAKE)
  file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_${p}DIR} _path )
  message(STATUS "** Installing ${p} components to ${_path}")
  unset(_path)
endforeach()


message(STATUS "** OPERATING SYSTEM ${CMAKE_OS_NAME}")


#message(FATAL_ERROR "** Testing to here")
#if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#  message(STATUS "** Configuring on/for Linux")
#  #target_compile_definitions(hello-world PUBLIC "IS_LINUX")
#elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#  message(STATUS "** Configuring on/for macOS")
#  #target_compile_definitions(hello-world PUBLIC "IS_MACOS")
#elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
#  message(STATUS "** Configuring on/for Windows")
#  #target_compile_definitions(hello-world PUBLIC "IS_WINDOWS")
#else()
#  message(STATUS "** Configuring on/for ${CMAKE_SYSTEM_NAME}")
#endif()


# DEVELOPER NOTE: This is a nice example to find libuuid using pkg_search_module
# Search for pkg-config and UUID
#find_package(PkgConfig QUIET)
#if(PKG_CONFIG_FOUND)
#  pkg_search_module(UUID uuid IMPORTED_TARGET)
#  if(TARGET PkgConfig::UUID)
#    message(STATUS "** Found libuuid")
#    set(UUID_FOUND TRUE)
#  endif()
#endif()

#message(FATAL_ERROR "** Testing to here")


###############################################################################
#####  We need a good C++ Compiler
###############################################################################
#set(CMAKE_BUILD_TYPE      Debug)
set(CXX_OPTIMIZE_LEVEL    "-O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS       "-frtti -fvisibility-inlines-hidden -fvisibility=hidden")
#set(CMAKE_CXX_FLAGS       "-Wall -Wextra")

set(CMAKE_CXX_STANDARD    11)
if(NOT DEFINED CMAKE_CXX_COMPILER)
  message (STATUS " ** Trying to find newer compiler")
  #set(MY_GCC_VER            "5.3")
  set(MY_GCC_VER            "8.2")
  #set(MY_BOOST_VER          "1.60.0")
  set(MY_BOOST_VER          "1.67.0")
  if(EXISTS "/usr/local/gcc-${MY_GCC_VER}/bin/g++-${MY_GCC_VER}")
    set(CMAKE_CXX_COMPILER    "/usr/local/gcc-${MY_GCC_VER}/bin/g++-${MY_GCC_VER}")
    set(BOOST_ROOT            "/usr/local/gcc-${MY_GCC_VER}_boost-${MY_BOOST_VER}/boost")
  else()
    if(EXISTS "/usr/local/gcc-${MY_GCC_VER}/bin/g++")
      set(CMAKE_CXX_COMPILER    "/usr/local/gcc-${MY_GCC_VER}/bin/g++")
    else()
      if(EXISTS "/usr/local/gcc/bin/g++")
        set(CMAKE_CXX_COMPILER     "/usr/local/gcc/bin/g++")
      endif()
    endif()
  endif()
endif()


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE        Release CACHE STRING "Build type" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE "-O2")
  set(CXX_OPTIMIZE_LEVEL      "${CMAKE_CXX_FLAGS_RELEASE}")
endif()


###############################################################################
#####  NOTE: We need to find out the version of the compiler that was used
###############################################################################
message(STATUS "** CXX COMPILER: ${COMPILER_NAME}")
execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -dumpversion OUTPUT_VARIABLE COMPILER_VERSION)
string(STRIP ${COMPILER_VERSION}   COMPILER_VERSION)
if(COMPILER_VERSION GREATER 7)
  set(CMAKE_CXX_STANDARD    17)
endif()
get_filename_component(COMPILER_NAME  ${CMAKE_CXX_COMPILER} NAME)


###############################################################################
#####  Get the Current FLOWER Version Number
###############################################################################
file(STRINGS ${CMAKE_SOURCE_DIR}/VERSION _VERSION_DATA)
string(REPLACE "_" ";" _VERSION_LIST ${_VERSION_DATA})
list(GET _VERSION_LIST 0 MAJOR_VERSION)
list(GET _VERSION_LIST 1 MINOR_VERSION)
list(GET _VERSION_LIST 2 MICRO_VERSION)


###############################################################################
#####  Get the header for creating FLOWER CSV files
###############################################################################
file(STRINGS ${CMAKE_SOURCE_DIR}/conf/flower_header.csv CSV_HEADER)


string(TIMESTAMP COMPILE_TIME)


###############################################################################
####  NOTE: For Debugging
###############################################################################
message(STATUS "#############################################################")
message(STATUS "** VERSION_DATA:            ${_VERSION_DATA}")
message(STATUS "** CURRENT_TIME:            ${COMPILE_TIME}")
message(STATUS "** Operating System:        ${CMAKE_OS_NAME}")
message(STATUS "** MAJOR_VERSION Include:   ${MAJOR_VERSION}")
message(STATUS "** MINOR_VERSION Include:   ${MINOR_VERSION}")
message(STATUS "** MICRO_VERSION Include:   ${MICRO_VERSION}")
message(STATUS "** CXX NAME:                ${COMPILER_NAME}")
message(STATUS "** CXX VERSION:             ${COMPILER_VERSION}")
message(STATUS "** CXX BUILD TYPE:          ${CMAKE_BUILD_TYPE}")
message(STATUS "** CXX OPTIMIZE LEVEL:      ${CXX_OPTIMIZE_LEVEL}")
message(STATUS "#############################################################")

add_executable(flower src/exe/flower.cpp)

###############################################################################
#####  Make sure we have BOOST Libraries
###############################################################################
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON)
find_package(Boost ${MY_BOOST_VER} REQUIRED COMPONENTS
    date_time
    filesystem
    program_options
    regex
    thread
    system
    unit_test_framework
)

if(Boost_FOUND)
  #  NOTE: For Debugging
  #
  #message(STATUS "** Boost Include:   ${Boost_INCLUDE_DIR}")
  #message(STATUS "** Boost Lib Dir:   ${Boost_LIBRARY_DIRS}")
  #message(STATUS "** Boost Libraries: ${Boost_LIBRARIES}")
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
  target_link_libraries(flower ${Boost_LIBRARIES} -static-libgcc -static-libstdc++)
endif()

###############################################################################
#####  Make sure we have PCAP Library
###############################################################################
find_package(PCAP REQUIRED)
if(PCAP_FOUND)
  message(STATUS "** PCAP  Include:   ${PCAP_INCLUDE_DIRS}")
  message(STATUS "** PCAP  Lib Dir:   ${PCAP_LIBRARIES}")
  include_directories(${PCAP_INCLUDE_DIRS})
  link_directories(${PCAP_LIBRARIES})
  target_link_libraries(flower ${PCAP_LIBRARIES} -static-libgcc -static-libstdc++)
endif()

#add_subdirectory(src/api)

###############################################################################
#####  Build FLOWER Library
###############################################################################
configure_file(${CMAKE_SOURCE_DIR}/include/version_info.hpp.in ${CMAKE_SOURCE_DIR}/include/version_info.hpp @ONLY)
add_library(flowerapi STATIC
                       src/api/date2string.cpp
                       src/api/Daemon.cpp
                       src/api/Diagnostic.cpp
                       src/api/Exception.cpp
                       src/api/ExceptionHandler.cpp
                       src/api/FlowCache.cpp
                       src/api/Flow.cpp
                       src/api/FlowKey.cpp
                       src/api/global.cpp
                       src/api/ItemCounter.cpp
                       src/api/num2string.cpp
                       src/api/osUtil.cpp
                       src/api/OutputHelper.cpp
                       src/api/PacketBuilder.cpp
                       src/api/Packet.cpp
                       src/api/PacketParser.cpp
                       src/api/PacketRinger.cpp
                       src/api/ProgramOptions.cpp
                       src/api/SummaryExporter.cpp
                       src/api/uchar2string.cpp
)


target_include_directories(flowerapi PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(flowerapi ${Boost_LIBRARIES} ${PCAP_LIBRARIES} -static-libgcc -static-libstdc++)


###############################################################################
#####  Build FLOWER Executable
###############################################################################
target_include_directories(flower PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(flower flowerapi -static-libgcc -static-libstdc++)
#set_target_properties(flower PROPERTIES
#                                        C_STANDARD 11
#                                        C_STANDARD_REQUIRED ON
#)


#get_cmake_property(variables VARIABLES)
#message(STATUS "VARIABLES: ${variables} :")
#get_cmake_property(cache_variables CACHE_VARIABLES)
#message(STATUS "CACHE_VARIABLES: ${cache_variables} :")
#get_cmake_property(commands COMMANDS)
#message(STATUS "COMMANDS: ${commands} :")
#get_cmake_property(macros MACROS)
#message(STATUS "MACROS: ${macros} :")
#get_cmake_property(components COMPONENTS)
#message(STATUS "COMPONENTS: ${components} :")
#message(STATUS "PLATFORM:  $<PLATFORM_ID> :")
#message(FATAL_ERROR "You can not do this at all, CMake will exit.")


###############################################################################
#####  Create Tests
###############################################################################
if (${ENABLE_TESTS})
  message(STATUS "** Tests will be built")
  enable_testing()

  foreach(testsourcefile
           src/test/t_bg_PacketRinger_root_loop_buffer.cpp
           src/test/t_bg_PacketRinger_root_loop_track.cpp
         )
    get_filename_component(t_filename ${testsourcefile} NAME_WE)
    add_executable(${t_filename} ${testsourcefile})
    target_include_directories(${t_filename} PRIVATE ${CMAKE_SOURCE_DIR}/include src/test src/test/packets)
    target_link_libraries(${t_filename} ${Boost_LIBRARIES} -static-libgcc -static-libstdc++)
    target_link_libraries(${t_filename} ${PCAP_LIBRARIES}  -static-libgcc -static-libstdc++)
    target_link_libraries(${t_filename} flowerapi          -static-libgcc -static-libstdc++)
    add_test(NAME ${t_filename}_test COMMAND ${t_filename} -- en0 100)
  endforeach(testsourcefile)

  foreach(testsourcefile
           src/test/t_bg_PacketRinger_root_noloop.cpp
           src/test/t_sudo_PacketBuilder.cpp
           src/test/t_SummaryExporter_root.cpp
         )
    get_filename_component(t_filename ${testsourcefile} NAME_WE)
    add_executable(${t_filename} ${testsourcefile})
    target_include_directories(${t_filename} PRIVATE ${CMAKE_SOURCE_DIR}/include src/test src/test/packets)
    target_link_libraries(${t_filename} ${Boost_LIBRARIES} -static-libgcc -static-libstdc++)
    target_link_libraries(${t_filename} ${PCAP_LIBRARIES}  -static-libgcc -static-libstdc++)
    target_link_libraries(${t_filename} flowerapi          -static-libgcc -static-libstdc++)
    add_test(NAME ${t_filename}_test COMMAND ${t_filename})
  endforeach(testsourcefile)

  foreach(testsourcefile
           src/test/t_bg_PacketRinger_user_noloop.cpp
           src/test/t_Data.cpp
           src/test/t_Elf.cpp
           src/test/t_Event.cpp
           src/test/t_FlowCache.cpp
           src/test/t_Flow.cpp
           src/test/t_Ip.cpp
           src/test/t_ObjectPool.cpp
           src/test/t_PacketBuilder.cpp
           src/test/t_Packet.cpp
           src/test/t_PacketParser.cpp
           src/test/t_ProgramOptions.cpp
         )
    get_filename_component(t_filename ${testsourcefile} NAME_WE)
    add_executable(${t_filename} ${testsourcefile})
    target_include_directories(${t_filename} PRIVATE ${CMAKE_SOURCE_DIR}/include src/test src/test/packets)
    target_link_libraries(${t_filename} ${Boost_LIBRARIES} -static-libgcc -static-libstdc++)
    target_link_libraries(${t_filename} ${PCAP_LIBRARIES}  -static-libgcc -static-libstdc++)
    target_link_libraries(${t_filename} flowerapi          -static-libgcc -static-libstdc++)
    add_test(NAME ${t_filename}_test COMMAND ${t_filename})
  endforeach(testsourcefile)
endif()


###############################################################################
#####  Install Files
###############################################################################
install(TARGETS             flower
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(FILES       docs/FLOWER_Data_Guide_FLR06.docx
                    docs/FLOWER_Install_Guide_FLR06.docx
                    docs/FLOWER_Ops_Guide_FLR06.docx
        DESTINATION docs)
install(FILES       conf/flower.conf
        DESTINATION conf)
install(FILES       scripts/create-flower-config.sh
        PERMISSIONS OWNER_EXECUTE OWNER_READ GROUP_READ GROUP_EXECUTE
        DESTINATION scripts)


###############################################################################
#####  Generate Packages
###############################################################################
string(TOLOWER ${PROJECT_NAME} PACKAGE_NAME)

set(CPACK_PACKAGE_NAME                 "${PACKAGE_NAME}")
set(CPACK_PACKAGE_DESCRIPTION          "FLOWER - Network FLOW analyzER")

# TODO:
#set(CPACK_RESOURCE_FILE_LICENSE        "${PROJECT_SOURCE_DIR}/LICENSE")
#set(CPACK_PACKAGE_DESCRIPTION_FILE     "${PROJECT_SOURCE_DIR}/INSTALL.md")

set(CPACK_SOURCE_IGNORE_FILES          "${PROJECT_BINARY_DIR};/.git/;.gitignore;docs/other")
set(CPACK_PACKAGING_INSTALL_PREFIX     "/opt/${PACKAGE_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR        "${MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR        "${MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH        "${MICRO_VERSION}")
set(CPACK_PACKAGE_VENDOR               "PNNL")
set(CPACK_PACKAGE_CONTACT              "FLOWER Support flower-support@pnnl.gov")
set(CPACK_PACKAGE_FILE_NAME            "${PACKAGE_NAME}-${MAJOR_VERSION}.${MINOR_VERSION}.${MICRO_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")

if(UNIX)
  if(CMAKE_SYSTEM_NAME MATCHES Linux)
    if(CMAKE_OS_NAME MATCHES ubuntu)
      list(APPEND CPACK_GENERATOR            "DEB")
      set(CPACK_DEBIAN_PACKAGE_MAINTAINER    "${CPACK_PACKAGE_CONTACT}")
      set(CPACK_DEBIAN_PACKAGE_SECTION       "other")
      set(CPACK_DEBIAN_PACKAGE_PRIORITY      "optional")
      set(CPACK_DEBIAN_PACKAGE_DEPENDS       "libpcap0.8")
    elseif(CMAKE_OS_NAME MATCHES centos)
      list(APPEND CPACK_GENERATOR            "RPM")
      set(CPACK_RPM_PACKAGE_RELEASE          "1")
      set(CPACK_RPM_PACKAGE_REQUIRES         "libpcap")
      SET(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/scripts/create-flower-config.sh")
    endif()
  endif()
endif()

include (CTest)
include (CPack)
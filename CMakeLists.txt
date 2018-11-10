cmake_minimum_required (VERSION 3.6)

project (FLOWER)

include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
message(STATUS "Project will be installed to ${CMAKE_INSTALL_PREFIX}")

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKEDIR CMake)
else()
  set(DEF_INSTALL_CMAKEDIR share/cmake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKEDIR ${DEF_INSTALL_CMAKEDIR} CACHE PATH "Installation directory for CMake files")

# Report to user
foreach(p LIB BIN INCLUDE CMAKE)
  file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_${p}DIR} _path )
  message(STATUS "Installing ${p} components to ${_path}")
  unset(_path)
endforeach()

# Search for pkg-config and UUID
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_search_module(UUID uuid IMPORTED_TARGET)
  if(TARGET PkgConfig::UUID)
    message(STATUS "Found libuuid")
    set(UUID_FOUND TRUE)
  endif()
endif()

#message(FATAL_ERROR "Testing to here")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")


enable_testing()

#set(CMAKE_BUILD_TYPE      Debug)
set(CXX_OPTIMIZE_LEVEL    "-O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS       "-Wall -Wextra")

set(CMAKE_CXX_STANDARD    11)
#set(CMAKE_CXX_STANDARD    17)
#set(MY_BOOST_VER          "1.60.0")
set(MY_BOOST_VER          "1.60.0")
if(NOT DEFINED CMAKE_CXX_COMPILER)
  #set(MY_GCC_VER            "5.3")
  set(MY_GCC_VER            "8.1")
  if(EXISTS "/usr/local/gcc-${MY_GCC_VER}/bin/g++-${MY_GCC_VER}")
    set(CMAKE_CXX_COMPILER    "/usr/local/gcc-${MY_GCC_VER}/bin/g++-${MY_GCC_VER}")
  else()
    if(EXISTS "/usr/local/gcc-${MY_GCC_VER}/bin/g++")
      set(CMAKE_CXX_COMPILER    "/usr/local/gcc-${MY_GCC_VER}/bin/g++")
    else()
      set(CMAKE_CXX_COMPILER     "/usr/local/gcc/bin/g++")
    endif()
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE "-O2")
  set(CXX_OPTIMIZE_LEVEL      "${CMAKE_CXX_FLAGS_RELEASE}")
endif()


#  NOTE: We need to find out the version of the compiler that was used
#
message(status "** CXX COMPILER: ${CMAKE_CXX_COMPILER}")
execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -dumpversion OUTPUT_VARIABLE GPP_VERSION)
string(STRIP ${GPP_VERSION}   GPP_VERSION)

file(STRINGS ${CMAKE_SOURCE_DIR}/VERSION _VERSION_DATA)
string(REPLACE "_" ";" _VERSION_LIST ${_VERSION_DATA})
list(GET _VERSION_LIST 0 MAJOR_VERSION)
list(GET _VERSION_LIST 1 MINOR_VERSION)
list(GET _VERSION_LIST 2 MICRO_VERSION)
message(status "** VERSION_DATA: ${_VERSION_DATA}")

string(TIMESTAMP COMPILE_TIME)
message(status "** CURRENT_TIME: ${COMPILE_TIME}")

#  NOTE: For Debugging
#
message(status "** MAJOR_VERSION Include:   ${MAJOR_VERSION}")
message(status "** MINOR_VERSION Include:   ${MINOR_VERSION}")
message(status "** MICRO_VERSION Include:   ${MICRO_VERSION}")
message(status "** CXX VERSION:             ${GPP_VERSION}")
message(status "** CXX BUILD TYPE:          ${CMAKE_BUILD_TYPE}")
message(status "** CXX OPTIMIZE LEVEL:      ${CXX_OPTIMIZE_LEVEL}")

#  NOTE: We need to find Boost libraries
#
set(BOOST_ROOT /usr/local/gcc-${MY_GCC_VER}_boost-${MY_BOOST_VER}/boost)
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

#  NOTE: For Debugging
#
#message(status "** Boost Include:   ${Boost_INCLUDE_DIR}")
#message(status "** Boost Lib Dir:   ${Boost_LIBRARY_DIRS}")
#message(status "** Boost Libraries: ${Boost_LIBRARIES}")

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
  add_executable(flower src/exe/flower.cpp)
  target_link_libraries(flower ${Boost_LIBRARIES} -static-libgcc -static-libstdc++)
endif()

find_package(PCAP QUIET)
target_link_libraries(flower ${PCAP_LIBRARY} -static-libgcc -static-libstdc++)

#add_subdirectory(src/api)

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
target_link_libraries(flowerapi ${Boost_LIBRARIES} -static-libgcc -static-libstdc++)
target_compile_definitions(flowerapi PRIVATE
                                      COMPILE_TIME=\"${COMPILE_TIME}\"
                                      BOOST_VER=\"${Boost_VERSION}\"
                                      MAJOR_VERSION=\"${MAJOR_VERSION}\"
                                      MINOR_VERSION=\"${MINOR_VERSION}\"
                                      MICRO_VERSION=\"${MICRO_VERSION}\"
                                      CXX_VER=\"${GPP_VERSION}\"
                                      CXX_DEBUG_LEVEL=\"${CMAKE_BUILD_TYPE}\"
                                      CXX_OPTIMIZE_LEVEL=\"${CXX_OPTIMIZE_LEVEL}\"
)

target_include_directories(flower PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(flower flowerapi -static-libgcc -static-libstdc++)
#set_target_properties(flower PROPERTIES
#                                        C_STANDARD 11
#                                        C_STANDARD_REQUIRED ON
#)


foreach(testsourcefile
         src/test/t_bg_PacketRinger_root_loop_buffer.cpp
         src/test/t_bg_PacketRinger_root_loop_track.cpp
         src/test/t_bg_PacketRinger_root_noloop.cpp
         src/test/t_bg_PacketRinger_user_noloop.cpp
         src/test/t_sudo_PacketBuilder.cpp
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
         src/test/t_SummaryExporter.cpp
       )
  get_filename_component(t_filename ${testsourcefile} NAME_WE)
  add_executable(${t_filename} ${testsourcefile})
  target_include_directories(${t_filename} PRIVATE ${CMAKE_SOURCE_DIR}/include src/test src/test/packets)
  target_link_libraries(${t_filename} ${Boost_LIBRARIES} -static-libgcc -static-libstdc++)
  target_link_libraries(${t_filename} ${PCAP_LIBRARY}    -static-libgcc -static-libstdc++)
  target_link_libraries(${t_filename} flowerapi          -static-libgcc -static-libstdc++)
  add_test(NAME ${t_filename}_test COMMAND ${t_filename})
endforeach(testsourcefile)


# Install ##################################################################
install(TARGETS flower
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(FILES docs/FLOWER_Data_Guide_FLR06.docx
              docs/FLOWER_Install_Guide_FLR06.docx
              docs/FLOWER_Ops_Guide_FLR06.docx
                               DESTINATION docs)
install(FILES conf/flower.conf DESTINATION conf)

# Package Generator  #######################################################
set(CPACK_PACKAGE_NAME                 "${PROJECT_NAME}")
set(CPACK_PACKAGE_DESCRIPTION          "FLOWER - Network FLOW analyzER")

# TODO:
#set(CPACK_PACKAGE_DESCRIPTION_SUMMARY  "message: a small messaging library")
#set(CPACK_RESOURCE_FILE_LICENSE        "${PROJECT_SOURCE_DIR}/LICENSE")
#set(CPACK_PACKAGE_DESCRIPTION_FILE     "${PROJECT_SOURCE_DIR}/INSTALL.md")

set(CPACK_SOURCE_IGNORE_FILES          "${PROJECT_BINARY_DIR};/.git/;.gitignore;docs/other")
set(CPACK_PACKAGING_INSTALL_PREFIX     "/opt/${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR        "${MAJOR_VERSION}")
set(CPACK_PACKAGE_VERSION_MINOR        "${MINOR_VERSION}")
set(CPACK_PACKAGE_VERSION_PATCH        "${MICRO_VERSION}")
set(CPACK_PACKAGE_VENDOR               "PNNL")
set(CPACK_PACKAGE_CONTACT              "FLOWER Support flower-support@pnnl.gov")
set(CPACK_PACKAGE_FILE_NAME            "${CMAKE_PROJECT_NAME}-${MAJOR_VERSION}.${MINOR_VERSION}.${MICRO_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")

if(UNIX)
 if(CMAKE_SYSTEM_NAME MATCHES Linux)
   list(APPEND CPACK_GENERATOR "DEB")
   set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
   set(CPACK_DEBIAN_PACKAGE_SECTION    "other")
   set(CPACK_DEBIAN_PACKAGE_PRIORITY   "optional")
   set(CPACK_DEBIAN_PACKAGE_DEPENDS    "libpcap0.8")
   list(APPEND CPACK_GENERATOR         "RPM")
   set(CPACK_RPM_PACKAGE_RELEASE       "1")
   set(CPACK_RPM_PACKAGE_REQUIRES      "libpcap")
  endif()
endif()

include (CPack)

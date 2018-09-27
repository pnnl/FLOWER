cmake_minimum_required (VERSION 3.6)

#find_package(PkgConfig QUIET)
#if(PKG_CONFIG_FOUND)
#  pkg_search_module(UUID uuid IMPORTED_TARGET)
#  if(TARGET PkgConfig::UUID)
#    message(STATUS "Found libuuid")
#    set(UUID_FOUND TRUE)
#  endif()
#endif()

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

add_library(flowerapi STATIC "")
target_sources(flowerapi PRIVATE
                ${CMAKE_CURRENT_LIST_DIR}/date2string.cpp
                ${CMAKE_CURRENT_LIST_DIR}/Daemon.cpp
                ${CMAKE_CURRENT_LIST_DIR}/Diagnostic.cpp
                ${CMAKE_CURRENT_LIST_DIR}/Exception.cpp
                ${CMAKE_CURRENT_LIST_DIR}/ExceptionHandler.cpp
                ${CMAKE_CURRENT_LIST_DIR}/FlowCache.cpp
                ${CMAKE_CURRENT_LIST_DIR}/Flow.cpp
                ${CMAKE_CURRENT_LIST_DIR}/FlowKey.cpp
                ${CMAKE_CURRENT_LIST_DIR}/global.cpp
                ${CMAKE_CURRENT_LIST_DIR}/ItemCounter.cpp
                ${CMAKE_CURRENT_LIST_DIR}/num2string.cpp
                ${CMAKE_CURRENT_LIST_DIR}/osUtil.cpp
                ${CMAKE_CURRENT_LIST_DIR}/OutputHelper.cpp
                ${CMAKE_CURRENT_LIST_DIR}/PacketBuilder.cpp
                ${CMAKE_CURRENT_LIST_DIR}/Packet.cpp
                ${CMAKE_CURRENT_LIST_DIR}/PacketParser.cpp
                ${CMAKE_CURRENT_LIST_DIR}/PacketRinger.cpp
                ${CMAKE_CURRENT_LIST_DIR}/ProgramOptions.cpp
                ${CMAKE_CURRENT_LIST_DIR}/SummaryExporter.cpp
                ${CMAKE_CURRENT_LIST_DIR}/uchar2string.cpp
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


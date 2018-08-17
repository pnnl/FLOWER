cmake_minimum_required (VERSION 3.6)
project (FLOWER)

#set(CMAKE_C_COMPILER     "/usr/local/gcc/bin/gcc")
set(CMAKE_CXX_COMPILER    "/usr/local/gcc/bin/g++")

#set(CMAKE_BUILD_TYPE      Debug)
set(CXX_OPTIMIZE_LEVEL    "-O0")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set(CMAKE_CXX_FLAGS       "-Wall -Wextra")

set(CMAKE_CXX_STANDARD    14)
#set(MY_BOOST_VER          "1.67.0")
#set(MY_GCC_VER            "8.1")
set(MY_BOOST_VER          "1.60.0")
set(MY_GCC_VER            "5.3")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE        Release)
  set(CMAKE_CXX_FLAGS_RELEASE "-O2")
  set(CXX_OPTIMIZE_LEVEL      "${CMAKE_CXX_FLAGS_RELEASE}")
endif()


#  NOTE: We need to find out the time the compile was executed
#        The best we can do is to find out when cmake was run last
execute_process(COMMAND date "+%Y-%m-%d-%H:%M" OUTPUT_VARIABLE COMPILE_TIME)
string(STRIP ${COMPILE_TIME}  COMPILE_TIME)

#  NOTE: We need to find out the Major, Minor, and Micro version numbers
#
execute_process(COMMAND cut -d "_" -f 1 VERSION.txt OUTPUT_VARIABLE MAJOR_VERSION)
execute_process(COMMAND cut -d "_" -f 2 VERSION.txt OUTPUT_VARIABLE MINOR_VERSION)
execute_process(COMMAND cut -d "_" -f 3 VERSION.txt OUTPUT_VARIABLE MICRO_VERSION)
string(STRIP "${MAJOR_VERSION}" MAJOR_VERSION)
string(STRIP "${MINOR_VERSION}" MINOR_VERSION)
string(STRIP "${MICRO_VERSION}" MICRO_VERSION)

#  NOTE: We need to find out the version of the compiler that was used
#
execute_process(COMMAND g++ -dumpversion OUTPUT_VARIABLE GPP_VERSION)
string(STRIP ${GPP_VERSION}   GPP_VERSION)

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
    program_options
    filesystem
    regex
    thread
    system
)

#  NOTE: For Debugging
#
#message(status "** Boost Include:   ${Boost_INCLUDE_DIR}")
#message(status "** Boost Lib Dir:   ${Boost_LIBRARY_DIRS}")
#message(status "** Boost Libraries: ${Boost_LIBRARIES}")

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
  add_executable(flower src/flower/exe/flower.cpp)
  target_link_libraries(flower ${Boost_LIBRARIES} -static-libgcc -static-libstdc++)
endif()

include(FindPCAP.cmake)
target_link_libraries(flower ${PCAP_LIBRARY} -static-libgcc -static-libstdc++)

add_library(flowerapi STATIC
                       src/flower/api/date2string.cpp
                       src/flower/api/Daemon.cpp
                       src/flower/api/Diagnostic.cpp
                       src/flower/api/Exception.cpp
                       src/flower/api/ExceptionHandler.cpp
                       src/flower/api/FlowCache.cpp
                       src/flower/api/Flow.cpp
                       src/flower/api/FlowKey.cpp
                       src/flower/api/global.cpp
                       src/flower/api/ItemCounter.cpp
                       src/flower/api/num2string.cpp
                       src/flower/api/osUtil.cpp
                       src/flower/api/OutputHelper.cpp
                       src/flower/api/PacketBuilder.cpp
                       src/flower/api/Packet.cpp
                       src/flower/api/PacketParser.cpp
                       src/flower/api/PacketRinger.cpp
                       src/flower/api/ProgramOptions.cpp
                       src/flower/api/SummaryExporter.cpp
                       src/flower/api/uchar2string.cpp
)

target_include_directories(flowerapi PRIVATE include)
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

target_include_directories(flower PRIVATE include)
#set_target_properties(flower PROPERTIES
#                                        C_STANDARD 11
#                                        C_STANDARD_REQUIRED ON
#)

target_link_libraries(flower flowerapi -static-libgcc -static-libstdc++)

cmake_minimum_required (VERSION 3.6)
project (FLOWER)

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD 11)

execute_process(COMMAND date "+%Y-%m-%d-%H:%M" OUTPUT_VARIABLE COMPILE_TIME)
execute_process(COMMAND awk -F'_' '{print $$1}' $(top_srcdir)/VERSION) OUTPUT_VARIABLE MAJOR_VERSION)
string(STRIP ${COMPILE_TIME}  COMPILE_TIME)
string(STRIP ${MAJOR_VERSION} MAJOR_VERSION)

set(Boost_USE_STATIC_LIBS       OFF) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.36.0 COMPONENTS date_time program_options filesystem regex system thread)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(flower src/flower/exe/flower.cpp)
  target_link_libraries(flower ${Boost_LIBRARIES})
endif()

#set_source_files_properties(./src/flower/api/ProgramOptions.cpp
#    PROPERTIES COMPILE_DEFINITIONS COMPILE_TIME=\"${COMPILE_TIME}\" BOOST_VER=\"${Boost_VERSION}\")

include(FindPCAP.cmake)
target_link_libraries(flower ${PCAP_LIBRARY})

add_library(libflower
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
target_include_directories(libflower PRIVATE include)
target_compile_definitions(libflower PRIVATE
                                      COMPILE_TIME=\"${COMPILE_TIME}\"
                                      BOOST_VER=\"${Boost_VERSION}\"
                                      
)

target_include_directories(flower PRIVATE include)
set_target_properties(flower PROPERTIES
                                        C_STANDARD 11
                                        C_STANDARD_REQUIRED ON
)

target_link_libraries(flower libflower)
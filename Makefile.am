#
#  Copyright (C) (2011-2021) Battelle Memorial Institute. All Rights Reserved.
#

ACLOCAL_AMFLAGS        =  -I m4 -I build-aux

AUTOMAKE_OPTIONS       =  foreign

EXTRA_DIST             =  m4/NOTES

COMPILETIME            =  $(shell date '+%s')
CXX_DBG_LEVEL          =  -g
CXX_OPT_LEVEL          =  -O3
CXXFLAGS               =  $(CXX_DBG_LEVEL) $(CXX_OPT_LEVEL) -std=c++11
CXX_VERSION            =  $(shell $(CXX) -v 2>&1 | grep version)

AM_CPPFLAGS            =
AM_CPPFLAGS           +=  -I$(top_srcdir)/include
#AM_CPPFLAGS           +=  -D_GLIBCXX_USE_CXX11_ABI=0
#AM_CPPFLAGS           +=  -Wno-deprecated-declarations


MAJOR_VERSION          =  $(shell $(AWK) -F'_' '{print $$1}' $(top_srcdir)/VERSION)
MINOR_VERSION          =  $(shell $(AWK) -F'_' '{print $$2}' $(top_srcdir)/VERSION)
MICRO_VERSION          =  $(shell $(AWK) -F'_' '{print $$3}' $(top_srcdir)/VERSION)
NEW_MAJOR_VERSION      =  $(shell $(AWK) -F'_' '{print $$1+1}' $(top_srcdir)/VERSION | bc)
NEW_MINOR_VERSION      =  $(shell $(AWK) -F'_' '{print $$2+1}' $(top_srcdir)/VERSION | bc)
NEW_MICRO_VERSION      =  $(shell $(AWK) -F'_' '{print $$3+1}' $(top_srcdir)/VERSION | bc)
BOOST_ROOT             =  $(shell echo $(BOOST_CPPFLAGS) | sed -e "s/^\s*\-I//" -e "s/\/include//")


BOOSTVERDEF            =  -D'BOOST_VER="$(BOOST_ROOT)"'
COMPILETIMEDEF         =  -D'COMPILE_TIME="$(COMPILETIME)"'
CXXOPTLEVELDEF         =  -D'CXX_OPTIMIZE_LEVEL="$(CXX_OPT_LEVEL)"'
CXXDBGLEVELDEF         =  -D'CXX_DEBUG_LEVEL="$(CXX_DBG_LEVEL)"'
CXXVERDEF              =  -D'CXX_VER="$(CXX_VERSION)"'
MAJORVERDEF            =  -D'MAJOR_VERSION="$(MAJOR_VERSION)"'
MINORVERDEF            =  -D'MINOR_VERSION="$(MINOR_VERSION)"'
MICROVERDEF            =  -D'MICRO_VERSION="$(MICRO_VERSION)"'


FLOWER_CPP_FLAGS       =
FLOWER_CPP_FLAGS      +=  $(AM_CPPFLAGS)
FLOWER_CPP_FLAGS      +=  $(BOOST_CPPFLAGS)
FLOWER_CPP_FLAGS      +=  $(BOOSTVERDEF)
FLOWER_CPP_FLAGS      +=  $(COMPILETIMEDEF)
FLOWER_CPP_FLAGS      +=  $(CXXOPTLEVELDEF)
FLOWER_CPP_FLAGS      +=  $(CXXDBGLEVELDEF)
FLOWER_CPP_FLAGS      +=  $(CXXVERDEF)
FLOWER_CPP_FLAGS      +=  $(MAJORVERDEF)
FLOWER_CPP_FLAGS      +=  $(MINORVERDEF)
FLOWER_CPP_FLAGS      +=  $(MICROVERDEF)

FLOWER_LD_FLAGS        =
FLOWER_LD_FLAGS       +=  $(BOOST_PROGRAM_OPTIONS_LDFLAGS)
FLOWER_LD_FLAGS       +=  $(BOOST_DATE_TIME_LDFLAGS)
FLOWER_LD_FLAGS       +=  $(BOOST_FILESYSTEM_LDFLAGS)
FLOWER_LD_FLAGS       +=  $(BOOST_REGEX_LDFLAGS)
FLOWER_LD_FLAGS       +=  $(BOOST_SYSTEM_LDFLAGS)
FLOWER_LD_FLAGS       +=  $(BOOST_THREAD_LDFLAGS)

FLOWER_LIBS            =
FLOWER_LIBS           +=  -lpcap
FLOWER_LIBS           +=  libflower.la
FLOWER_LIBS           +=  $(BOOST_PROGRAM_OPTIONS_LIBS)
FLOWER_LIBS           +=  $(BOOST_DATE_TIME_LIBS)
FLOWER_LIBS           +=  $(BOOST_FILESYSTEM_LIBS)
FLOWER_LIBS           +=  $(BOOST_REGEX_LIBS)
FLOWER_LIBS           +=  $(BOOST_SYSTEM_LIBS)
FLOWER_LIBS           +=  $(BOOST_THREAD_LIBS)

FLOWER_TCPP_FLAGS      =  $(FLOWER_CPP_FLAGS)
FLOWER_TCPP_FLAGS     +=  -I$(top_srcdir)/src/flower/api/test

FLOWER_TLD_FLAGS       =  $(FLOWER_LD_FLAGS)
FLOWER_TLD_FLAGS      +=  $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS)

FLOWER_TLDADD          =  $(FLOWER_LIBS)
FLOWER_TLDADD         +=  $(BOOST_UNIT_TEST_FRAMEWORK_LIBS)


lib_LTLIBRARIES        =  libflower.la


libflower_la_SOURCES   =
libflower_la_SOURCES  +=  src/flower/api/date2string.cpp
libflower_la_SOURCES  +=  src/flower/api/Daemon.cpp
libflower_la_SOURCES  +=  src/flower/api/Diagnostic.cpp
libflower_la_SOURCES  +=  src/flower/api/Exception.cpp
libflower_la_SOURCES  +=  src/flower/api/ExceptionHandler.cpp
libflower_la_SOURCES  +=  src/flower/api/FlowCache.cpp
libflower_la_SOURCES  +=  src/flower/api/Flow.cpp
libflower_la_SOURCES  +=  src/flower/api/FlowKey.cpp
libflower_la_SOURCES  +=  src/flower/api/global.cpp
libflower_la_SOURCES  +=  src/flower/api/ItemCounter.cpp
libflower_la_SOURCES  +=  src/flower/api/num2string.cpp
libflower_la_SOURCES  +=  src/flower/api/osUtil.cpp
libflower_la_SOURCES  +=  src/flower/api/OutputHelper.cpp
libflower_la_SOURCES  +=  src/flower/api/PacketBuilder.cpp
libflower_la_SOURCES  +=  src/flower/api/Packet.cpp
libflower_la_SOURCES  +=  src/flower/api/PacketParser.cpp
libflower_la_SOURCES  +=  src/flower/api/PacketRinger.cpp
libflower_la_SOURCES  +=  src/flower/api/ProgramOptions.cpp
libflower_la_SOURCES  +=  src/flower/api/SummaryExporter.cpp
libflower_la_SOURCES  +=  src/flower/api/uchar2string.cpp

libflower_la_CPPFLAGS  =  $(FLOWER_CPP_FLAGS)

libflower_la_LDFLAGS   =  $(FLOWER_LD_FLAGS)

libflower_la_LIBS      =  $(FLOWER_LIBS)


bin_PROGRAMS           =  flower


flower_SOURCES         =  src/flower/exe/flower.cpp

flower_CPPFLAGS        =  $(FLOWER_CPP_FLAGS)

flower_LDFLAGS         =  $(FLOWER_LD_FLAGS)

flower_LDADD           =  $(FLOWER_LIBS)

#
#  TEST CODE
#

bin_PROGRAMS          +=  t_Data
t_Data_SOURCES         =  src/flower/api/test/t_Data.cpp
t_Data_CPPFLAGS        =  $(FLOWER_TCPP_FLAGS)
t_Data_LDFLAGS         =  $(FLOWER_TLD_FLAGS)
t_Data_LDADD           =  $(FLOWER_TLDADD)


bin_PROGRAMS          +=  t_Elf
t_Elf_SOURCES          =  src/flower/api/test/t_Elf.cpp
t_Elf_CPPFLAGS         =  $(FLOWER_TCPP_FLAGS)
t_Elf_LDFLAGS          =  $(FLOWER_TLD_FLAGS)
t_Elf_LDADD            =  $(FLOWER_TLDADD)


bin_PROGRAMS          +=  t_Event
t_Event_SOURCES        =  src/flower/api/test/t_Event.cpp
t_Event_CPPFLAGS       =  $(FLOWER_TCPP_FLAGS)
t_Event_LDFLAGS        =  $(FLOWER_TLD_FLAGS)
t_Event_LDADD          =  $(FLOWER_TLDADD)


bin_PROGRAMS          +=  t_Flow
t_Flow_SOURCES         =  src/flower/api/test/t_Flow.cpp
t_Flow_CPPFLAGS        =  $(FLOWER_TCPP_FLAGS)
t_Flow_LDFLAGS         =  $(FLOWER_TLD_FLAGS)
t_Flow_LDADD           =  $(FLOWER_TLDADD)


bin_PROGRAMS          +=  t_FlowCache
t_FlowCache_SOURCES    =  src/flower/api/test/t_FlowCache.cpp
t_FlowCache_CPPFLAGS   =  $(FLOWER_TCPP_FLAGS)
t_FlowCache_LDFLAGS    =  $(FLOWER_TLD_FLAGS)
t_FlowCache_LDADD      =  $(FLOWER_TLDADD)


bin_PROGRAMS          +=  t_Ip
t_Ip_SOURCES           =  src/flower/api/test/t_Ip.cpp
t_Ip_CPPFLAGS          =  $(FLOWER_TCPP_FLAGS)
t_Ip_LDFLAGS           =  $(FLOWER_TLD_FLAGS)
t_Ip_LDADD             =  $(FLOWER_TLDADD)


bin_PROGRAMS          +=  t_ObjectPool
t_ObjectPool_SOURCES   =  src/flower/api/test/t_ObjectPool.cpp
t_ObjectPool_CPPFLAGS  =  $(FLOWER_TCPP_FLAGS)
t_ObjectPool_LDFLAGS   =  $(FLOWER_TLD_FLAGS)
t_ObjectPool_LDADD     =  $(FLOWER_TLDADD)


bin_PROGRAMS             +=  t_Packet
t_Packet_SOURCES          =  src/flower/api/test/t_Packet.cpp
t_Packet_CPPFLAGS         =  $(FLOWER_TCPP_FLAGS)
t_Packet_LDFLAGS          =  $(FLOWER_TLD_FLAGS)
t_Packet_LDADD            =  $(FLOWER_TLDADD)


bin_PROGRAMS             +=  t_PacketBuilder
t_PacketBuilder_SOURCES   =  src/flower/api/test/t_PacketBuilder.cpp
t_PacketBuilder_CPPFLAGS  =  $(FLOWER_TCPP_FLAGS)
t_PacketBuilder_LDFLAGS   =  $(FLOWER_TLD_FLAGS)
t_PacketBuilder_LDADD     =  $(FLOWER_TLDADD)


bin_PROGRAMS             +=  t_PacketParser
t_PacketParser_SOURCES    =  src/flower/api/test/t_PacketParser.cpp
t_PacketParser_CPPFLAGS   =  $(FLOWER_TCPP_FLAGS)
t_PacketParser_LDFLAGS    =  $(FLOWER_TLD_FLAGS)
t_PacketParser_LDADD      =  $(FLOWER_TLDADD)


bin_PROGRAMS             +=  t_ProgramOptions
t_ProgramOptions_SOURCES  =  src/flower/api/test/t_ProgramOptions.cpp
t_ProgramOptions_CPPFLAGS =  $(FLOWER_TCPP_FLAGS)
t_ProgramOptions_LDFLAGS  =  $(FLOWER_TLD_FLAGS)
t_ProgramOptions_LDADD    =  $(FLOWER_TLDADD)


bin_PROGRAMS              +=  t_SummaryExporter
t_SummaryExporter_SOURCES  =  src/flower/api/test/t_SummaryExporter.cpp
t_SummaryExporter_CPPFLAGS =  $(FLOWER_TCPP_FLAGS)
t_SummaryExporter_LDFLAGS  =  $(FLOWER_TLD_FLAGS)
t_SummaryExporter_LDADD    =  $(FLOWER_TLDADD)


bin_PROGRAMS                                +=  t_bg_PacketRinger_root_loop_buffer
t_bg_PacketRinger_root_loop_buffer_SOURCES   =  src/flower/api/test/t_bg_PacketRinger_root_loop_buffer.cpp
t_bg_PacketRinger_root_loop_buffer_CPPFLAGS  =  $(FLOWER_TCPP_FLAGS)
t_bg_PacketRinger_root_loop_buffer_LDFLAGS   =  $(FLOWER_TLD_FLAGS)
t_bg_PacketRinger_root_loop_buffer_LDADD     =  $(FLOWER_TLDADD)


bin_PROGRAMS                                +=  t_bg_PacketRinger_root_loop_track
t_bg_PacketRinger_root_loop_track_SOURCES    =  src/flower/api/test/t_bg_PacketRinger_root_loop_track.cpp
t_bg_PacketRinger_root_loop_track_CPPFLAGS   =  $(FLOWER_TCPP_FLAGS)
t_bg_PacketRinger_root_loop_track_LDFLAGS    =  $(FLOWER_TLD_FLAGS)
t_bg_PacketRinger_root_loop_track_LDADD      =  $(FLOWER_TLDADD)


bin_PROGRAMS                                +=  t_bg_PacketRinger_root_noloop
t_bg_PacketRinger_root_noloop_SOURCES        =  src/flower/api/test/t_bg_PacketRinger_root_noloop.cpp
t_bg_PacketRinger_root_noloop_CPPFLAGS       =  $(FLOWER_TCPP_FLAGS)
t_bg_PacketRinger_root_noloop_LDFLAGS        =  $(FLOWER_TLD_FLAGS)
t_bg_PacketRinger_root_noloop_LDADD          =  $(FLOWER_TLDADD)
 

bin_PROGRAMS                                +=  t_bg_PacketRinger_user_noloop
t_bg_PacketRinger_user_noloop_SOURCES        =  src/flower/api/test/t_bg_PacketRinger_user_noloop.cpp
t_bg_PacketRinger_user_noloop_CPPFLAGS       =  $(FLOWER_TCPP_FLAGS)
t_bg_PacketRinger_user_noloop_LDFLAGS        =  $(FLOWER_TLD_FLAGS)
t_bg_PacketRinger_user_noloop_LDADD          =  $(FLOWER_TLDADD)

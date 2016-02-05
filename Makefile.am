ACLOCAL_AMFLAGS      = -I m4 -I build-aux

AUTOMAKE_OPTIONS     = foreign

#SUBDIRS          =                                                            \
#                   src/flower/api                                             \
#                   src/flower/exe

EXTRA_DIST           = m4/NOTES


AM_CPPFLAGS          =  -DNDEBUG=1 -I$(top_srcdir)/include -I/share/apps/libpcap/1.7.4/include

lib_LTLIBRARIES      =  libflower.la
libflower_la_SOURCES =                                                        \
                        src/flower/api/Daemon.cpp                             \
                        src/flower/api/date2string.cpp                        \
                        src/flower/api/Diagnostic.cpp                         \
                        src/flower/api/diagnostic.cpp                         \
                        src/flower/api/Exception.cpp                          \
                        src/flower/api/ExceptionHandler.cpp                   \
                        src/flower/api/FlowCache.cpp                          \
                        src/flower/api/Flow.cpp                               \
                        src/flower/api/FlowKey.cpp                            \
                        src/flower/api/global.cpp                             \
                        src/flower/api/ItemCounter.cpp                        \
                        src/flower/api/num2string.cpp                         \
                        src/flower/api/osUtil.cpp                             \
                        src/flower/api/OutputHelper.cpp                       \
                        src/flower/api/PacketBuilder.cpp                      \
                        src/flower/api/Packet.cpp                             \
                        src/flower/api/PacketParser.cpp                       \
                        src/flower/api/PacketRinger.cpp                       \
                        src/flower/api/ProgramOptions.cpp                     \
                        src/flower/api/SummaryExporter.cpp                    \
                        src/flower/api/uchar2string.cpp

libflower_la_CPPFLAGS = $(AM_CPPFLAGS)

bin_PROGRAMS         =  flower
flower_SOURCES       =                                                        \
                        src/flower/exe/flower.cpp

flower_CPPFLAGS      =  $(AM_CPPFLAGS) $(BOOST_CPPFLAGS)
flower_LDFLAGS       =  $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(BOOST_FILESYSTEM_LDFLAGS) $(BOOST_REGEX_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_THREAD_LDFLAGS)
flower_LDADD         =  $(lib_LTLIBRARIES)
flower_LIBS          =  $(BOOST_PROGRAM_OPTIONS_LIBS) $(BOOST_FILESYSTEM_LIBS) $(BOOST_REGEX_LIBS) $(BOOST_SYSTEM_LIBS) $(BOOST_THREAD_LIBS)

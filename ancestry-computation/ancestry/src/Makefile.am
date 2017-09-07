
bin_PROGRAMS = ancestry relfinder
DISTCHECK_CONFIGURE_FLAGS=LDFLAGS=-L/opt/local/lib CXXFLAGS=-I/opt/local/include
ancestry_SOURCES = ancestry.cpp Data23.cpp Ancestry_params.cpp CmdLine.cpp gzstream.cpp FreqData.cpp
relfinder_SOURCES = relfinder.cpp Data23.cpp Ancestry_params.cpp CmdLine.cpp gzstream.cpp FreqData.cpp

noinst_HEADERS = gzstream.h FreqData.h CmdLine.h Data23.h Ancestry_params.h

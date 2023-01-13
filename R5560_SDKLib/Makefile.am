# this is example-file: R5560_SDKLib/Makefile.am

# additional include paths necessary to compile the C++ library
AUTOMAKE_OPTIONS = subdir-objects
AM_CXXFLAGS = -I$(top_srcdir)/R5560_SDKLib @AM_CXXFLAGS@

###############################################################################
# THE LIBRARIES TO BUILD
###############################################################################

lib_LTLIBRARIES = libr5560.la

libr5560_la_LDFLAGS = -version-info @VERSION_INFO@

# where to install the headers on the system
libr5560_ladir = $(includedir)/r5560

# the list of header files that belong to the library (to be installed later)
libr5560_la_HEADERS =    \
                        R5560_SDKLib.h 

# the sources to add to the library and to add to the source distribution
libr5560_la_SOURCES =    \
                        $(libbr5560_la_HEADERS) \
                        R5560_SDKLib.cpp 
						
###############################################################################
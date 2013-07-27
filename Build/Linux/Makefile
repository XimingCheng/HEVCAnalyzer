# -------------------------------------------------------------------------
# These are configurable options:
# -------------------------------------------------------------------------

# C++ compiler 
CXX = clang++
# CXX = g++

# Standard flags for C++ 
CXXFLAGS ?= 

# Standard preprocessor flags (common for CC and CXX) 
CPPFLAGS ?= 

# Standard linker flags, the wxsqlite3 lib version is specified as follow
# You may want to change the wxsqlite3 lib version
LDFLAGS ?= -lX11 -lwxsqlite3-2.8

# Location and arguments of wx-config script 
WX_CONFIG ?= wx-config

# C++ flags to use with wxWidgets code 
WX_CXXFLAGS ?= `$(WX_CONFIG) --cxxflags`

### Variables: ###
HEVCANALYZER_SRCDIR = ../../HEVCAnalyzer
LIBCOMMON_SRCDIR    = ../../TLibCommon
LIBVIDEOIO_SRCDIR   = ../../TLibVideoIO
HEVCANALYZER_OBJDIR = ./HEVCAnalyzerObj
LIBCOMMON_OBJDIR    = ./TLibCommonObj
LIBVIDEOIO_OBJDIR   = ./TLibVideoIOObj
BIN_DIR             = ../../bin

DEBUGFLAG = -ggdb
CPPDEPS = -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MMD -MP
HEVCANALYZER_CXXFLAGS =  -Wall -Wno-unknown-pragmas -I.  $(WX_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)
HEVCANALYZER_OBJECTS =  \
	$(HEVCANALYZER_OBJDIR)/HEVCodecApp.o \
	$(HEVCANALYZER_OBJDIR)/HEVCAnalyzer.o \
	$(HEVCANALYZER_OBJDIR)/MainFrame.o \
	$(HEVCANALYZER_OBJDIR)/ThumbnailThread.o \
	$(HEVCANALYZER_OBJDIR)/YUVConfigDlg.o \
	$(HEVCANALYZER_OBJDIR)/PicViewCtrl.o \
	$(HEVCANALYZER_OBJDIR)/PixelViewCtrl.o \
	$(HEVCANALYZER_OBJDIR)/RulerCtrl.o \
	$(LIBCOMMON_OBJDIR)/TComPicYuv.o \
	$(LIBCOMMON_OBJDIR)/TComRom.o \
	$(LIBVIDEOIO_OBJDIR)/TVideoIOYuv.o

HEVCANALYZER_OBJECTS_D = $(patsubst %.o, %_d.o, $(HEVCANALYZER_OBJECTS))

### Conditionally set variables: ###



### Targets: ###

all: debug release

debug: makedir $(BIN_DIR)/HEVCAnalyzer_d

release: makedir $(BIN_DIR)/HEVCAnalyzer

install: 

uninstall: 

clean: 
	rm -f $(HEVCANALYZER_OBJDIR)/*.o
	rm -f $(HEVCANALYZER_OBJDIR)/*.d
	rm -f $(LIBCOMMON_OBJDIR)/*.o
	rm -f $(LIBCOMMON_OBJDIR)/*.d
	rm -f $(LIBVIDEOIO_OBJDIR)/*.o
	rm -f $(LIBVIDEOIO_OBJDIR)/*.d
	rm -f $(BIN_DIR)/HEVCAnalyzer
	rm -f $(BIN_DIR)/HEVCAnalyzer_d

$(BIN_DIR)/HEVCAnalyzer: $(HEVCANALYZER_OBJECTS)
	$(CXX) -o $@ $(HEVCANALYZER_OBJECTS)   `$(WX_CONFIG) --libs html,aui,core,base` $(LDFLAGS)

$(BIN_DIR)/HEVCAnalyzer_d: $(HEVCANALYZER_OBJECTS_D)
	$(CXX) -o $@ $(HEVCANALYZER_OBJECTS_D)   `$(WX_CONFIG) --libs html,aui,core,base` $(LDFLAGS) $(DEBUGFLAG)

$(HEVCANALYZER_OBJDIR)/%.o: $(HEVCANALYZER_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<
$(LIBCOMMON_OBJDIR)/%.o: $(LIBCOMMON_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<
$(LIBVIDEOIO_OBJDIR)/%.o: $(LIBVIDEOIO_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<

$(HEVCANALYZER_OBJDIR)/%_d.o: $(HEVCANALYZER_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)
$(LIBCOMMON_OBJDIR)/%_d.o: $(LIBCOMMON_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)
$(LIBVIDEOIO_OBJDIR)/%_d.o: $(LIBVIDEOIO_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)

makedir:$(HEVCANALYZER_OBJDIR) $(LIBCOMMON_OBJDIR) $(LIBVIDEOIO_OBJDIR) $(BIN_DIR)

$(HEVCANALYZER_OBJDIR):
	mkdir -p $(HEVCANALYZER_OBJDIR)
$(LIBCOMMON_OBJDIR):
	mkdir -p $(LIBCOMMON_OBJDIR)
$(LIBVIDEOIO_OBJDIR):
	mkdir -p $(LIBVIDEOIO_OBJDIR)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)


.PHONY: all install uninstall clean

# Dependencies tracking
-include $(HEVCANALYZER_OBJDIR)/*.d $(LIBCOMMON_OBJDIR)/*.d $(LIBVIDEOIO_OBJDIR)/*.d

# -------------------------------------------------------------------------
# These are configurable options:
# -------------------------------------------------------------------------

# C++ compiler 
CXX = g++

# Standard flags for C++ 
CXXFLAGS ?= 

# Standard preprocessor flags (common for CC and CXX) 
CPPFLAGS ?= 

# Standard linker flags 
LDFLAGS ?= 

# Location and arguments of wx-config script 
WX_CONFIG ?= wx-config

# C++ flags to use with wxWidgets code 
WX_CXXFLAGS ?= `$(WX_CONFIG) --cxxflags`

### Variables: ###
HEVCANALYZER_SRCDIR = ../../HEVCAnalyzer
LIBCOMMON_SRCDIR    = ../../TLibCommon
LIBVIDEOIO_SRCDIR   = ../../TLibVideoIO
HEVCANALYZER_OBJDIR = ./HEVCAnalyzer
LIBCOMMON_OBJDIR    = ./TLibCommon
LIBVIDEOIO_OBJDIR   = ./TLibVideoIO
BIN_DIR             = ../../bin

DEBUGFLAG = -ggdb
CPPDEPS = -MT$@ -MF`echo $@ | sed -e 's,\.o$$,.d,'` -MD -MP
HEVCANALYZER_CXXFLAGS =  -I.  $(WX_CXXFLAGS) $(CPPFLAGS) $(CXXFLAGS)
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

HEVCANALYZER_OBJECTS_D =  \
	$(HEVCANALYZER_OBJDIR)/HEVCodecApp_d.o \
	$(HEVCANALYZER_OBJDIR)/HEVCAnalyzer_d.o \
	$(HEVCANALYZER_OBJDIR)/MainFrame_d.o \
	$(HEVCANALYZER_OBJDIR)/ThumbnailThread_d.o \
	$(HEVCANALYZER_OBJDIR)/YUVConfigDlg_d.o \
	$(HEVCANALYZER_OBJDIR)/PicViewCtrl_d.o \
	$(HEVCANALYZER_OBJDIR)/PixelViewCtrl_d.o \
	$(HEVCANALYZER_OBJDIR)/RulerCtrl_d.o \
	$(LIBCOMMON_OBJDIR)/TComPicYuv_d.o \
	$(LIBCOMMON_OBJDIR)/TComRom_d.o \
	$(LIBVIDEOIO_OBJDIR)/TVideoIOYuv_d.o

### Conditionally set variables: ###



### Targets: ###

all: makedir debug release

debug: HEVCAnalyzer_d

release: HEVCAnalyzer

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

HEVCAnalyzer: $(HEVCANALYZER_OBJECTS)
	$(CXX) -o $(BIN_DIR)/$@ $(HEVCANALYZER_OBJECTS)   `$(WX_CONFIG) --libs html,aui,core,base` $(LDFLAGS)

HEVCAnalyzer_d: $(HEVCANALYZER_OBJECTS_D)
	$(CXX) -o $(BIN_DIR)/$@ $(HEVCANALYZER_OBJECTS_D)   `$(WX_CONFIG) --libs html,aui,core,base` $(LDFLAGS) $(DEBUGFLAG)

$(HEVCANALYZER_OBJDIR)/%.o: $(HEVCANALYZER_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<
$(LIBCOMMON_OBJDIR)/%.o: $(LIBCOMMON_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<
$(LIBVIDEOIO_OBJDIR)/%.o: $(LIBVIDEOIO_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<

$(HEVCANALYZER_OBJDIR)/%_d.o: $(HEVCANALYZER_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)
$(LIBCOMMON_OBJDIR)/%_d.o: $(LIBCOMMON_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<
$(LIBVIDEOIO_OBJDIR)/%_d.o: $(LIBVIDEOIO_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<

makedir:
	@if [ ! -d $(HEVCANALYZER_OBJDIR) ]; then\
		mkdir $(HEVCANALYZER_OBJDIR);\
	fi
	@if [ ! -d $(LIBCOMMON_OBJDIR) ]; then\
		mkdir $(LIBCOMMON_OBJDIR);\
	fi
	@if [ ! -d $(LIBVIDEOIO_OBJDIR) ]; then\
		mkdir $(LIBVIDEOIO_OBJDIR);\
	fi
	@if [ ! -d $(BIN_DIR) ]; then\
		mkdir $(BIN_DIR);\
	fi

.PHONY: all install uninstall clean


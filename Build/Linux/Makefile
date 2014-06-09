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
APPCOMMOM_SRCDIR    = ../../TAppCommon
APPDECODER_SRCDIR   = ../../TAppDecoder
LIBDECODER_SRCDIR   = ../../TLibDecoder
LIBMD5_SRCDIR       = ../../libmd5

HEVCANALYZER_OBJDIR = ./HEVCAnalyzerObj
LIBCOMMON_OBJDIR    = ./TLibCommonObj
LIBVIDEOIO_OBJDIR   = ./TLibVideoIOObj
APPCOMMOM_OBJDIR    = ./TAppCommonObj
APPDECODER_OBJDIR   = ./TAppDecoderObj
LIBDECODER_OBJDIR   = ./TLibDecoderObj
LIBMD5_OBJDIR       = ./libmd5Obj
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
	$(HEVCANALYZER_OBJDIR)/MainUIInstance.o \
	$(LIBCOMMON_OBJDIR)/ContextModel3DBuffer.o \
	$(LIBCOMMON_OBJDIR)/ContextModel.o \
	$(LIBCOMMON_OBJDIR)/SEI.o \
	$(LIBCOMMON_OBJDIR)/TComBitStream.o \
	$(LIBCOMMON_OBJDIR)/TComCABACTables.o \
	$(LIBCOMMON_OBJDIR)/TComDataCU.o \
	$(LIBCOMMON_OBJDIR)/TComInterpolationFilter.o \
	$(LIBCOMMON_OBJDIR)/TComLoopFilter.o \
	$(LIBCOMMON_OBJDIR)/TComMotionInfo.o \
	$(LIBCOMMON_OBJDIR)/TComPattern.o \
	$(LIBCOMMON_OBJDIR)/TComPic.o \
	$(LIBCOMMON_OBJDIR)/TComPicSym.o \
	$(LIBCOMMON_OBJDIR)/TComPicYuv.o \
	$(LIBCOMMON_OBJDIR)/TComPicYuvMD5.o \
	$(LIBCOMMON_OBJDIR)/TComPrediction.o \
	$(LIBCOMMON_OBJDIR)/TComRdCost.o \
	$(LIBCOMMON_OBJDIR)/TComRdCostWeightPrediction.o \
	$(LIBCOMMON_OBJDIR)/TComRom.o \
	$(LIBCOMMON_OBJDIR)/TComSampleAdaptiveOffset.o \
	$(LIBCOMMON_OBJDIR)/TComSlice.o \
	$(LIBCOMMON_OBJDIR)/TComTrQuant.o \
	$(LIBCOMMON_OBJDIR)/TComWeightPrediction.o \
	$(LIBCOMMON_OBJDIR)/TComYuv.o \
	$(LIBVIDEOIO_OBJDIR)/TVideoIOYuv.o \
	$(APPCOMMOM_OBJDIR)/program_options_lite.o \
	$(APPDECODER_OBJDIR)/TAppDecCfg.o \
	$(APPDECODER_OBJDIR)/TAppDecTop.o \
	$(LIBDECODER_OBJDIR)/AnnexBread.o \
	$(LIBDECODER_OBJDIR)/NALread.o \
	$(LIBDECODER_OBJDIR)/SEIread.o \
	$(LIBDECODER_OBJDIR)/SyntaxElementParser.o \
	$(LIBDECODER_OBJDIR)/TDecBinCoderCABAC.o \
	$(LIBDECODER_OBJDIR)/TDecCAVLC.o \
	$(LIBDECODER_OBJDIR)/TDecCu.o \
	$(LIBDECODER_OBJDIR)/TDecEntropy.o \
	$(LIBDECODER_OBJDIR)/TDecGop.o \
	$(LIBDECODER_OBJDIR)/TDecSbac.o \
	$(LIBDECODER_OBJDIR)/TDecSlice.o \
	$(LIBDECODER_OBJDIR)/TDecTop.o \
	$(LIBMD5_OBJDIR)/libmd5.o

HEVCANALYZER_OBJECTS_D = $(patsubst %.o, %_d.o, $(HEVCANALYZER_OBJECTS))

### Conditionally set variables: ###



### Targets: ###

all: debug release

debug: makedir $(BIN_DIR)/HEVCAnalyzer_d

release: makedir $(BIN_DIR)/HEVCAnalyzer

install: 

uninstall: 

clean: 
#	rm -f $(HEVCANALYZER_OBJDIR)/*.o
#	rm -f $(HEVCANALYZER_OBJDIR)/*.d
#	rm -f $(LIBCOMMON_OBJDIR)/*.o
#	rm -f $(LIBCOMMON_OBJDIR)/*.d
#	rm -f $(LIBVIDEOIO_OBJDIR)/*.o
#	rm -f $(LIBVIDEOIO_OBJDIR)/*.d
#	rm -f $(APPCOMMOM_OBJDIR)/*.o
#	rm -f $(APPCOMMOM_OBJDIR)/*.d
#	rm -f $(APPDECODER_OBJDIR)/*.o
#	rm -f $(APPDECODER_OBJDIR)/*.d
#	rm -f $(LIBDECODER_SRCDIR)/*.o
#	rm -f $(LIBDECODER_SRCDIR)/*.d
#	rm -f $(LIBMD5_OBJDIR)/*.o
#	rm -f $(LIBMD5_OBJDIR)/*.d
#	rm -f $(BIN_DIR)/HEVCAnalyzer
#	rm -f $(BIN_DIR)/HEVCAnalyzer_d
	rm -rf $(BIN_DIR)
	rm -rf $(HEVCANALYZER_OBJDIR)
	rm -rf $(LIBCOMMON_OBJDIR)
	rm -rf $(LIBDECODER_OBJDIR)
	rm -rf $(LIBVIDEOIO_OBJDIR)
	rm -rf $(APPCOMMOM_OBJDIR)
	rm -rf $(LIBMD5_OBJDIR)
	rm -rf $(APPDECODER_OBJDIR)

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
$(APPCOMMOM_OBJDIR)/%.o: $(APPCOMMOM_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<
$(APPDECODER_OBJDIR)/%.o: $(APPDECODER_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<
$(LIBDECODER_OBJDIR)/%.o: $(LIBDECODER_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<
$(LIBMD5_OBJDIR)/%.o: $(LIBMD5_SRCDIR)/%.c
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $<

$(HEVCANALYZER_OBJDIR)/%_d.o: $(HEVCANALYZER_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)
$(LIBCOMMON_OBJDIR)/%_d.o: $(LIBCOMMON_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)
$(LIBVIDEOIO_OBJDIR)/%_d.o: $(LIBVIDEOIO_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)
$(APPCOMMOM_OBJDIR)/%_d.o: $(APPCOMMOM_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)
$(APPDECODER_OBJDIR)/%_d.o: $(APPDECODER_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)
$(LIBDECODER_OBJDIR)/%_d.o: $(LIBDECODER_SRCDIR)/%.cpp
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)
$(LIBMD5_OBJDIR)/%_d.o: $(LIBMD5_SRCDIR)/%.c
	$(CXX) -c -o $@ $(HEVCANALYZER_CXXFLAGS) $(CPPDEPS) $< $(DEBUGFLAG)

makedir:$(HEVCANALYZER_OBJDIR) $(LIBCOMMON_OBJDIR) $(LIBVIDEOIO_OBJDIR) $(APPCOMMOM_OBJDIR) $(APPDECODER_OBJDIR) $(LIBDECODER_OBJDIR) $(LIBMD5_OBJDIR) $(BIN_DIR)

$(HEVCANALYZER_OBJDIR):
	mkdir -p $(HEVCANALYZER_OBJDIR)
$(LIBCOMMON_OBJDIR):
	mkdir -p $(LIBCOMMON_OBJDIR)
$(LIBVIDEOIO_OBJDIR):
	mkdir -p $(LIBVIDEOIO_OBJDIR)
$(APPCOMMOM_OBJDIR):
	mkdir -p $(APPCOMMOM_OBJDIR)
$(APPDECODER_OBJDIR):
	mkdir -p $(APPDECODER_OBJDIR)
$(LIBDECODER_OBJDIR):
	mkdir -p $(LIBDECODER_OBJDIR)
$(LIBMD5_OBJDIR):
	mkdir -p $(LIBMD5_OBJDIR)
$(BIN_DIR):
	mkdir -p $(BIN_DIR)


.PHONY: all install uninstall clean

# Dependencies tracking
-include $(HEVCANALYZER_OBJDIR)/*.d $(LIBCOMMON_OBJDIR)/*.d $(LIBVIDEOIO_OBJDIR)/*.d $(APPCOMMOM_OBJDIR)/*.d $(APPDECODER_OBJDIR)/*.d $(LIBDECODER_OBJDIR)/*.d $(LIBMD5_OBJDIR)/*.d

.PHONY: all test clean

OFLAGS = $(if $(DEBUG),-O0 -g,-O3 -fno-omit-frame-pointer -g)
export CPPFLAGS = -I$(BOOST_INC) \
  -I$(CANVAS_INC) \
  -I$(CETLIB_INC) \
  -I$(FHICLCPP_INC) \
  -I$(GALLERY_INC) \
  -I$(LARCOREOBJ_INC) \
  -I$(LARDATAOBJ_INC) \
  -I$(NUSIMDATA_INC) \
  -I$(ROOT_INC)

comma = ,

export UNAME_S := $(shell uname -s)
export DYN_LIB_PATH = $(if $(filter Darwin,$(UNAME_S)),DY,)LD_LIBRARY_PATH

UNDEF_FLAG = $(if $(filter Darwin,$(UNAME_S)),-Wl$(comma)-undefined$(comma)error,-Wl$(comma)--no-undefined)

export CXXFLAGS = -fPIC -std=c++14 -Wall -Werror -Wextra -pedantic $(OFLAGS)
export CXX = g++
export LDFLAGS = $$(root-config --libs) \
  -L$(CANVAS_LIB) -lcanvas_Utilities -lcanvas_Persistency_Common -lcanvas_Persistency_Provenance \
  -L$(CETLIB_LIB) -lcetlib \
  -L$(GALLERY_LIB) -lgallery \
  -L$(NUSIMDATA_LIB) -lnusimdata_SimulationBase \
  -L$(LARCOREOBJ_LIB) -llarcoreobj_SummaryData \
  -L$(LARDATAOBJ_LIB) -llardataobj_RecoBase \
  $(UNDEF_FLAG)

LIB := libhdf5_art_explore.so
OBJECTS := compare.o
EXEC := for_dana

all : $(EXEC)

for_dana.o : compare.hh

$(EXEC) : % : %.o $(LIB)
	@echo Building $(@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LIB) -o $@ $<

compare.o : compare.hh

libhdf5_art_explore.so: compare.o
	@echo Building $(@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -fPIC -shared -o $(@) $(^) 

clean:
	@$(MAKE) -C test clean
	-@$(RM) compare.o libhdf5_art_explore.so $(EXEC).o $(EXEC)
	-@$(RM) -r *.dSYM

test: all
	@$(MAKE) -C test

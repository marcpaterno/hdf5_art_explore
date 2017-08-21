TESTS := compare_assns_t compare_vertex_t compare_cluster_t \
         compare_hit_t compare_hitcoll_t compare_trajectory_t \
         compare_particle_t compare_neutrino_t compare_truth_t

all : $(TESTS)

.PHONY: test clean $(foreach t,$(TESTS),test-$(t))

CPPFLAGS += -I..

test: $(foreach t,$(TESTS),test-$(t))

$(foreach t,$(TESTS),test-$(t)) : test-% : %
	@printf "Testing $(@) ... "
	@$(DYN_LIB_PATH)=.:..$${$(DYN_LIB_PATH):+:$${$(DYN_LIB_PATH)}} ./$(^) && \
     echo "OK" || { echo "FAILED!"; false; }

compare_%.o : ../compare.hh

compare_assns_t.o : compare_assns_t.hh

compare_assns_t : libtest_dict.so ../libhdf5_art_explore.so

$(foreach i,vertex cluster hit hitcoll trajectory particle neutrino truth,compare_$(i)_t) : ../libhdf5_art_explore.so

$(foreach i,trajectory particle neutrino truth,compare_$(i)_t) : LDFLAGS += -L$(ROOTSYS)/lib -lPhysics

$(TESTS): LDFLAGS += -L$(ROOTSYS)/lib -lCore

$(TESTS): % : %.o
	@echo Building $(@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $(^)

comma = ,

libtest_dict.so: test_dict.o ../libhdf5_art_explore.so
	@echo Building $(@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -fPIC -shared -o $@ $^ -L $(ROOTSYS)/lib -lCore \
$(if $(filter Darwin,$(UNAME_S)),,-Wl$(comma)-rpath-link$(comma)..)

test_dict.cpp: classes.h classes_def.xml
	@echo Generating $(@)
	genreflex $(^) --noIncludePaths -I. $(filter -I% -D%,$(CPPFLAGS)) \
   -l libtest_dict.so --rootmap-lib=libtest_dict.so --rootmap=libtest_dict.rootmap \
   -o $(@)

clean:
	-@$(RM) libtest_dict_rdict.pcm test_dict.cpp libtest_dict.so libtest_dict.rootmap *.o $(TESTS)
	-@$(RM) -r *.dSYM

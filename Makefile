cc=g++

HELPER=benchmarks/tasks.hpp

ifndef $(BOOST)
	BOOST=~/boost_1_82_0/
endif

ifndef $(ONETBB)
	ONETBB=~/my_onetbb/include
endif

boost_template: benchmarks/boost_template.cpp
	$(cc) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread


boost_scalability: benchmarks/boost_scalability.cpp
	$(cc) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread

tbb_template: benchmarks/tbb_template.cpp
	$(cc) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_scalability: benchmarks/tbb_scalability.cpp
	$(cc) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

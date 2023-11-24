cc=g++
CFLAGS= -std=c++20 -Wall -Wextra
HELPER=benchmarks/tasks.hpp

ifndef $(BOOST)
	BOOST=~/boost_1_82_0/
endif

ifndef $(ONETBB)
	ONETBB=~/my_onetbb/include
endif

boost_template: benchmarks/boost_template.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread


boost_scalability: benchmarks/boost_scalability.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread

boost_blocking: benchmarks/boost_blocking_tasks.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread

tbb_template: benchmarks/tbb_template.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_scalability: benchmarks/tbb_scalability.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_blocking: benchmarks/tbb_blocking_tasks.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

pool_scalability: benchmarks/pool_scalability.cpp
	$(cc) $(CFLAGS) $(HELPER) $< -o $@ 
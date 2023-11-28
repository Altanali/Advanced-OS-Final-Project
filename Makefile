cc=g++
CFLAGS= -std=c++20 -Wall -Wextra
HELPER=benchmarks/tasks.hpp

ifndef $(BOOST)
	BOOST=~/boost_1_82_0/
endif

ifndef $(ONETBB)
	ONETBB=~/my_onetbb/include
endif

ifndef $(BS)
	BS=~/thread-pool/include
endif

POOL = src/thread_safe_queue.hpp src/threadpool.hpp

boost_template: benchmarks/boost_template.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread

boost_scalability: benchmarks/boost_scalability.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread

boost_blocking: benchmarks/boost_blocking_tasks.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread

boost_dependent: benchmarks/boost_dependent.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread	

tbb_template: benchmarks/tbb_template.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_scalability: benchmarks/tbb_scalability.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_blocking: benchmarks/tbb_blocking_tasks.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_dependent: benchmarks/tbb_dependent.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

bs_scalability: benchmarks/bs_scalability.cpp $(HELPER)
	$(cc) $(CFLAGS) -I $(BS) $< $(HELPER) -o $@

bs_blocking: benchmarks/boost_blocking_tasks.cpp $(HELPER)
	$(cc) $(CFLAGS) -I $(BS) $< $(HELPER) -o $@

bs_dependent: benchmarks/bs_dependent.cpp $(HELPER)
	$(cc) $(CFLAGS) -I $(BS) $< $(HELPER) -o $@

pool_scalability: benchmarks/pool_scalability.cpp
	$(cc) $(CFLAGS) $(HELPER) $(POOL) $< -o $@ 

pool_dependent: benchmarks/pool_dependent.cpp
	$(cc) $(CFLAGS) $(HELPER) $(POOL) $< -o $@ 

pool_blocking: benchmarks/pool_blocking_tasks.cpp
	$(cc) $(CFLAGS) $(HELPER) $(POOL) $< -o $@ 

nopool_scalability: benchmarks/nopool_scalability.cpp
	$(cc) $(CFLAGS) $(HELPER) $< -o $@
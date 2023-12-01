cc=g++
CFLAGS= -std=c++20 
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

boost_benchmarks=benchmarks/boost
tbb_benchmarks=benchmarks/tbb
bs_benchmarks=benchmarks/bs
pool_benchmarks=benchmarks/pool
nopool_benchmarks=benchmarks/nopool

POOL = src/thread_safe_queue.hpp src/threadpool.hpp

#boost benchmarks

boost_template: $(boost_benchmarks)/boost_template.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread

boost_scalability: $(boost_benchmarks)/boost_scalability.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread

boost_blocking: $(boost_benchmarks)/boost_blocking_tasks.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread

boost_dependent: $(boost_benchmarks)/boost_dependent.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread	

boost_small: $(boost_benchmarks)/boost_small_tasks.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread	

boost_tasks: $(boost_benchmarks)/boost_scale_tasks.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread	

boost_mixed_tasks: $(boost_benchmarks)/boost_mixed_tasks.cpp
	$(cc) $(CFLAGS) -I $(BOOST) $< $(HELPER) -o $@ -lpthread -lboost_thread

#tbb benchmarks

tbb_template: $(tbb_benchmarks)/tbb_template.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_scalability: $(tbb_benchmarks)/tbb_scalability.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_blocking: $(tbb_benchmarks)/tbb_blocking_tasks.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_dependent: $(tbb_benchmarks)/tbb_dependent.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_small: $(tbb_benchmarks)/tbb_small_tasks.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_tasks: $(tbb_benchmarks)/tbb_scale_tasks.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_mixed_tasks: $(tbb_benchmarks)/tbb_mixed_tasks.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb

tbb_nested: $(tbb_benchmarks)/tbb_nested.cpp
	$(cc) $(CFLAGS) -I $(ONETBB) $< $(HELPER) -o $@ -ltbb


#bs benchmarks

bs_scalability: $(bs_benchmarks)/bs_scalability.cpp $(HELPER)
	$(cc) $(CFLAGS) -I $(BS) $< $(HELPER) -o $@

bs_blocking: $(bs_benchmarks)/bs_blocking_tasks.cpp $(HELPER)
	$(cc) $(CFLAGS) -I $(BS) $< $(HELPER) -o $@

bs_dependent: $(bs_benchmarks)/bs_dependent.cpp $(HELPER)
	$(cc) $(CFLAGS) -I $(BS) $< $(HELPER) -o $@

bs_small: $(bs_benchmarks)/bs_small_tasks.cpp $(HELPER)
	$(cc) $(CFLAGS) -I $(BS) $< $(HELPER) -o $@

bs_tasks: $(bs_benchmarks)/bs_scale_tasks.cpp $(HELPER)
	$(cc) $(CFLAGS) -I $(BS) $< $(HELPER) -o $@


bs_nested: $(bs_benchmarks)/bs_nested.cpp $(HELPER)
	$(cc) $(CFLAGS) -I $(BS) $< $(HELPER) -o $@


#pool benchmarks

pool_scalability: $(pool_benchmarks)/pool_scalability.cpp
	$(cc) $(CFLAGS) $(HELPER) $(POOL) $< -o $@ 

pool_dependent: $(pool_benchmarks)/pool_dependent.cpp
	$(cc) $(CFLAGS) $(HELPER) $(POOL) $< -o $@ 

pool_blocking: $(pool_benchmarks)/pool_blocking_tasks.cpp
	$(cc) $(CFLAGS) $(HELPER) $(POOL) $< -o $@ 

pool_small: $(pool_benchmarks)/pool_small_tasks.cpp
	$(cc) $(CFLAGS) $(HELPER) $(POOL) $< -o $@ 


#no pool benchmarks 

nopool_scalability: $(nopool_benchmarks)/nopool_scalability.cpp
	$(cc) $(CFLAGS) $(HELPER) $< -o $@

nopool_blocking: $(nopool_benchmarks)/nopool_blocking_tasks.cpp
	$(cc) $(CFLAGS) $(HELPER) $< -o $@

nopool_small: $(nopool_benchmarks)/nopool_small_tasks.cpp
	$(cc) $(CFLAGS) $(HELPER) $< -o $@

nopool_dependent: $(nopool_benchmarks)/nopool_dependent.cpp
	$(cc) $(CFLAGS) $(HELPER) $< -o $@
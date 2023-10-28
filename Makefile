cc=g++


ifndef $(BOOST)
	BOOST=~/boost_1_82_0/
endif

ifndef $(ONETBB)
	ONETBB=~/my_onetbb/include
endif

boost_template: benchmarks/boost_template.cpp
	$(cc) -I $(BOOST) $< -o $@ -lpthread -lboost_thread


tbb_template: benchmarks/tbb_template.cpp
	$(cc) -I $(ONETBB) $< -o $@ -ltbb

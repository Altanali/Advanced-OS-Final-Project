cc=g++


ifndef $(BOOST)
	BOOST=~/boost_1_82_0/
endif


boost_template: benchmarks/boost_template.cpp
	$(cc) -I $(BOOST) $< -o $@ -lpthread -lboost_thread-mt




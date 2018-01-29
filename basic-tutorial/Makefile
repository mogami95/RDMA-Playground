
CXX=g++
CPPFLAGS=-std=c++11 #-w
HEADER=#-I/data/miao/lib/libibverbs-1.2.0/include
LIBB=#-L/data/miao/lib/libibverbs-1.2.0/lib
LDFLAGS=-lzmq -libverbs
#ibv_devinfo -v

all: rdma_test benchmark

rdma_resouce.o: rdma_resource.hpp rdma_resource.cpp
	$(CXX) $(CPPFLAGS) $(HEADER) $(LIBB) $(LDFLAGS) rdma_resource.cpp -c

rdma_test: rdma_test.cpp rdma_resource.o
	$(CXX) $(CPPFLAGS) $(HEADER) $(LIBB) $(LDFLAGS) rdma_test.cpp -o rdma_test rdma_resource.o

benchmark: benchmark.cpp rdma_resource.o
	$(CXX) $(CPPFLAGS) $(HEADER) $(LIBB) $(LDFLAGS) benchmark.cpp -o benchmark rdma_resource.o

clean:
	-rm rdma_test benchmark

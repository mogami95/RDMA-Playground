CCOMPILE=mpic++
PLATFORM=Linux-amd64-64
CPPFLAGS=-std=c++11 -O2 -Wno-deprecated 
LDFLAGS=-lpthread -lrt -lzmq -libverbs -lrdma

all: run

run: run.cpp
	$(CCOMPILE) run.cpp $(CPPFLAGS) $(LDFLAGS)  -o run

clean:
	rm run

# scp  -r /home/miaoloud/Projects/WK-COMM/ miao@proj99:/data/miao

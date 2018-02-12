#include "rdma_resource.hpp"
#include <iostream>

int main(int argc, char** argv)
{
	assert(argc == 3);
	std::string remote_name = argv[1];
	int port = 12333;
	std::string role = argv[2];
	
	int size = 1024;
	void* mem = malloc(size);

	// create an RdmaResourcePair
	RdmaResourcePair rdma((char*)mem, size);
	// exchange the info
	rdma.exchange_info(remote_name, port);

	// test rdma write
	// writer writes data to reader
	{
		if (role == "writer")
		{
			char* msg = rdma.get_buf();
			strcpy(msg, "hello world");
			// the last parameter is the virtual address of remote machine
			rdma.rdma_write(msg, strlen(msg), 0);
			rdma.poll_completion();
		}
		else if (role == "reader")
		{
			char* res;
			rdma.busy_read(&res);
			printf("Message received: %s\n", res);
		}
		else
		{
			assert(false);
		}
	}
	std::cout << std::endl;

	// test rdma read
	// reader reads data from writer
	{
		const char* data = "Come and read my data!";
		if (role == "writer")
		{
			char* msg = rdma.get_buf();
			strcpy(msg, data);
			printf("writer got writer's buffer set\n");
			printf("writer waits the reader's rdma_read calling\n");
		}
		
		// wait writer's buffer done
		rdma.barrier(remote_name, port);

		if (role == "reader")
		{
			char* msg = rdma.get_buf();
			rdma.rdma_read(msg, strlen(data), 0);
			rdma.poll_completion();
			printf("Message received: %s\n", msg);
		}
	}
	std::cout << std::endl;
	
	return 0;
}

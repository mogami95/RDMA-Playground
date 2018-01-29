#include "rdma_resource.hpp"
#include "zmq.hpp"

#include <iostream>
#include <chrono>

using namespace std::chrono;


RdmaResourcePair rdma_setup(std::string remote_name, int port, char* mem, size_t size)
{
	RdmaResourcePair rdma(mem, size);
	rdma.exchange_info(remote_name, port);
	return rdma;
}

void rdma_communicate_send(RdmaResourcePair& rdma, std::string remote_name, int port, std::string role)
{
	if (role == "reader")
	{
		rdma.post_receive(rdma.get_buf(), rdma.get_buf_size());
	}

	rdma.barrier(remote_name, port);

	if (role == "writer")
	{
		rdma.send(rdma.get_buf(), rdma.get_buf_size(), 0);
		rdma.poll_completion();
		std::cout << "[Testing] Message sent successfully." << std::endl;
	}
	else if (role == "reader")
	{
		rdma.poll_completion();
		std::cout << "[Testing] Message received successfully." << std::endl;
		// msg was stored in rdma.get_buf() actually.
	}
	else
	{
		assert(false);
	}
}

void rdma_communicate_write_imm(RdmaResourcePair& rdma, std::string remote_name, int port, std::string role)
{
	if (role == "reader")
		rdma.post_receive(rdma.get_buf(), 1);

	rdma.barrier(remote_name, port);

	if (role == "writer")
	{
		rdma.rdma_write_imm(rdma.get_buf(), rdma.get_buf_size(), 0);
		rdma.poll_completion();
		std::cout << "[Testing] Message wrote successfully." << std::endl;
	}
	else if (role == "reader")
	{
		rdma.poll_completion();
		std::cout << "[Testing] Message received successfully." << std::endl;
		// msg was stored in rdma.get_buf() actually.
	}
	else
	{
		assert(false);
	}
}

void test_rdma(std::string remote_name, int port, std::string role, char* mem, size_t size, int times)
{
	// setup rdma connection
	RdmaResourcePair rdma = rdma_setup(remote_name, port, mem, size);

	// communicate
	for (int i = 0; i < times; ++ i)
	{
		auto t1 = steady_clock::now();
		rdma_communicate_write_imm(rdma, remote_name, port, role);
		auto t2 = steady_clock::now();
		auto dura = duration_cast<duration<double>>(t2-t1);
		std::cout << "[Testing] Round time: " << dura.count() << " seconds." << std::endl;
	}
}

zmq::socket_t zmq_setup(zmq::context_t& ctx, std::string remote_name, int port, std::string role)
{
	if (role == "reader")
	{
		zmq::socket_t pull(ctx, ZMQ_PULL);
		pull.bind("tcp://*:" + std::to_string(port));
		return pull;
	}
	else if (role == "writer")
	{
		zmq::socket_t push(ctx, ZMQ_PUSH);
		push.connect("tcp://" + remote_name + ":" + std::to_string(port));
		return push;
	}
}

void zmq_communicate(zmq::socket_t& zmq_socket, std::string role, char* mem, size_t size)
{
	if (role == "reader")
	{
		zmq::message_t msg;
		zmq_socket.recv(&msg);
		std::cout << "[Testing] Message received successfully." << std::endl;
	}
	else if (role == "writer")
	{
		zmq::message_t msg(mem, size);
		zmq_socket.send(msg);
		std::cout << "[Testing] Message sent successfully." << std::endl;
	}
}

void test_zmq(std::string remote_name, int port, std::string role, char* mem, size_t size, int times)
{
	// setup zmq connection
	zmq::context_t ctx;
	zmq::socket_t zmq_socket = zmq_setup(ctx, remote_name, port, role);
	std::cout << "[Testing] zmq setup done" << std::endl;

	// communiate
	for (int i = 0; i < times; ++ i)
	{
		auto t1 = steady_clock::now();
		zmq_communicate(zmq_socket, role, mem, size);
		auto t2 = steady_clock::now();
		auto dura = duration_cast<duration<double>>(t2-t1);
		std::cout << "[Testing] Round time: " << dura.count() << " seconds." << std::endl;
	}
	
	free(mem);
}

int main(int argc, char** argv)
{
	assert(argc == 3);
	std::string remote_name = argv[1];
	int port = 12333;
	std::string role = argv[2];

	// generate a very large string
	constexpr size_t megabyte = 1048576;
	constexpr size_t gigabyte = 1073741824;
	constexpr size_t size = gigabyte;
	std::cout << "[Testing] Testing size : " << size << std::endl;

	char* mem_rdma = (char*)malloc(size);
	memset(mem_rdma, '\0', size);
	char* mem_zmq = (char*)malloc(size);
	memset(mem_zmq, '\0', size);
	std::cout << "[Testing] string generated" << std::endl;

	std::cout << "[Testing] rdma" << std::endl;
	test_rdma(remote_name, port, role, mem_rdma, size, 3);

	std::cout << "[Testing] zmq" << std::endl;
	test_zmq(remote_name, port, role, mem_zmq, size, 3);
}

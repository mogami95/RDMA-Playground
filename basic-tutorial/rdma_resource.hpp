#ifndef RDMA_RESOURCE_HPP_
#define RDMA_RESOURCE_HPP_

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include <byteswap.h>
#include <endian.h>
#include <getopt.h>
#include <inttypes.h>
#include <netdb.h>
#include <stdint.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include <zmq.hpp>

#include <infiniband/verbs.h>

/* structure for config */
struct config_t
{
	const char *dev_name;	/* IB device name */
	char *server_name;		/* server host name */
	int ib_port;			/* local IB port to work with */
	int gid_idx;			/* gid index to use */
};

/* structure to exchange data which is needed to connect the QPs */
struct cm_con_data_t
{
	uint64_t addr;		/* Buffer address */
	uint32_t rkey;		/* Remote key */
	uint32_t qp_num;	/* QP number */
	uint16_t lid;		/* LID of the IB port */
	uint8_t gid[16];	/* gid */
} __attribute__ ((packed));

/* structure of system resources */
struct dev_resource
{
	struct ibv_device_attr device_attr;		/* Device attributes */
	struct ibv_port_attr port_attr;			/* IB port attributes */

	struct ibv_context *ib_ctx;		/* device handle */
	struct ibv_pd *pd;				/* PD handle */
	struct ibv_mr *mr;				/* MR handle for buf */
	char *buf;						/* memory buffer pointer, used for RDMA and send */

	dev_resource() : ib_ctx(NULL), pd(NULL), mr(NULL), buf(NULL) {}
};

struct QP
{
	struct cm_con_data_t remote_props;	/* values to connect to remote side */
	
	struct dev_resource *dev;
	struct ibv_pd *pd;	/* PD handle */
	struct ibv_mr *mr;	/* MR handle for buf */
	
	struct ibv_qp *qp;	/* QP handle */
	struct ibv_cq *cq;	/* CQ handle */

	QP() : dev(NULL), pd(NULL), mr(NULL), qp(NULL), cq(NULL) {}

};

class RdmaResourcePair
{
public:
	RdmaResourcePair(char* _mem, uint32_t _size);
	~RdmaResourcePair();

	// exchange the QP info between
	void exchange_info(const std::string& remote_name, int port);

	void post_receive(char* local, uint32_t size);

	void poll_completion();
	
	void send(char* local_buf, uint32_t size, uint64_t remote_offset);
	void send_imm(char* local_buf, uint32_t size, uint64_t remote_offset);
	void rdma_write(char* local_buf, uint32_t size, uint64_t remote_offset);
	void rdma_write_imm(char* local_buf, uint32_t size, uint64_t remote_offset);
	void rdma_read(char* local_buf, uint32_t size, uint64_t remote_offset);
	// void rdma_fetch_add(int t_id, int m_id, char *local, uint32_t size, uint64_t remote_offset, uint64_t add);
	// void rdma_cmp_swap(int t_id, int m_id, char *local, uint32_t size, uint64_t remote_offset, uint64_t compare, uint64_t swap);

	// get the register buffer and size
	char* get_buf();
	uint32_t get_buf_size();
	void busy_read(char** res);
	
	// for barrier
	void barrier(const std::string& remote_name, int port);

private:

	void rdma_op(char* local_buf, uint32_t size, uint64_t remote_offset, int op);

	// for debug
	void print_conn(const cm_con_data_t& data);

	dev_resource* dev;  // device
	QP* res;			// qp
	enum ibv_qp_type qp_type;

	char *buffer;	// The ptr of registed buffer
	uint32_t size;	// The size of the rdma region,should be the same across machines!

};


#endif

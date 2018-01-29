/*
 * Copyright (c) 2016 Shanghai Jiao Tong University.
 *     All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS
 *  IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *  express or implied.  See the License for the specific language
 *  governing permissions and limitations under the License.
 *
 * For more about this software visit:
 *
 *      http://ipads.se.sjtu.edu.cn/projects/wukong
 *
 */

#pragma once

#include "util.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


#ifdef HAS_RDMA

#include "rdmaio.h"
using namespace rdmaio;

class RDMA
{
private:

	struct RDMA_Device
	{
		RdmaCtrl* ctrl;

		RDMA_Device(int num_nodes, int num_threads, int nid, char *mem, uint64_t mem_sz, string ipfn, int tcp_port) : ctrl(NULL)
		{
			// record IPs of ndoes
			vector<string> ipset;
			ifstream ipfile(ipfn);
			string ip;
			while(!ipfile.eof())
			{
				getline(ipfile, ip);
				ipset.push_back(ip);
				if(ipfile.eof())
					break;
			}
			//cout<<"ipset.size()="<<ipset.size()<<" at workerID="<<_my_rank<<endl;

			worker_barrier();

			// initialization of new librdma
			// nid, ipset, port, thread_id-no use, enable single memory region
			ctrl = new RdmaCtrl(nid, ipset, tcp_port, true);
			ctrl->open_device();
			ctrl->set_connect_mr(mem, mem_sz);
			ctrl->register_connect_mr();//single
			ctrl->start_server();
			for (uint j = 0; j < num_threads; ++j)
			{
				for (uint i = 0; i < num_nodes; ++i)
				{
					Qp *qp = ctrl->create_rc_qp(j, i, 0, 1);
					assert(qp != NULL);
				}
			}

			while (1)
			{
				int connected = 0;
				for (uint j = 0; j < num_threads; ++j)
				{
					for (uint i = 0; i < num_nodes; ++i)
					{
						Qp *qp = ctrl->create_rc_qp(j, i, 0, 1);
						if (qp->inited_) connected += 1;
						else
						{
							if (qp->connect_rc())
							{
								connected += 1;
							}
						}
					}
				}
				if (connected == num_nodes * num_threads)
					break;
				else
					sleep(1);
			}

			//cout<<"ctrl->qps_.size()="<<ctrl->qps_.size()<<" at workerID="<<get_worker_id()<<endl;
		}

		~RDMA_Device()
		{
			if (ctrl != NULL) delete ctrl;
		}

		void RdmaRead(int dst_tid, int dst_nid, char *local, uint64_t size, uint64_t off)
		{
			Qp* qp = ctrl->get_rc_qp(dst_tid, dst_nid);

			int flags = IBV_SEND_SIGNALED;

			qp->rc_post_send(IBV_WR_RDMA_READ, local, size, off, flags);
			if (!qp->first_send()) qp->poll_completion();
			qp->poll_completion();
		}

		void RdmaWrite(int dst_tid, int dst_nid, char *local, uint64_t size, uint64_t off)
		{
			Qp* qp = ctrl->get_rc_qp(dst_tid, dst_nid);

			int flags = IBV_SEND_SIGNALED;

			qp->rc_post_send(IBV_WR_RDMA_WRITE, local, size, off, flags);
			qp->poll_completion();
		}

		void RdmaWriteSelective(int dst_tid, int dst_nid, char *local, uint64_t size, uint64_t off)
		{
			Qp* qp = ctrl->get_rc_qp(dst_tid, dst_nid);

			int flags = (qp->first_send() ? IBV_SEND_SIGNALED : 0);

			qp->rc_post_send(IBV_WR_RDMA_WRITE, local, size, off, flags);
			if (qp->need_poll()) qp->poll_completion();
		}

		void RdmaWriteNonSignal(int dst_tid, int dst_nid, char *local, uint64_t size, uint64_t off)
		{
			Qp* qp = ctrl->get_rc_qp(dst_tid, dst_nid);

			int flags = 0;

			qp->rc_post_send(IBV_WR_RDMA_WRITE, local, size, off, flags);
		}
	};

public:

	RDMA_Device *dev;

	RDMA() : dev(NULL) { }

	~RDMA()
	{
		if (dev != NULL) delete dev;
	}

	void init_rdma(int num_nodes, int num_threads, int nid, char *mem, uint64_t mem_sz, string ipfn, int tcp_port)
	{
		dev = new RDMA_Device(num_nodes, num_threads, nid, mem, mem_sz, ipfn, tcp_port);
	}

	inline static RDMA &get_rdma()
	{
		static RDMA rdma;
		return rdma;
	}

	inline static bool has_rdma()
	{
		return true;
	}
};


#else

class RDMA
{
	class RDMA_Device
	{
	public:
		RDMA_Device(int num_nodes, int num_threads, int nid, char *mem, uint64_t mem_sz, string fname)
		{
			cout << "This system is compiled without RDMA support." << endl;
			assert(false);
		}

		int RdmaRead(int dst_tid, int dst_nid, char *local, uint64_t size, uint64_t remote_offset)
		{
			cout << "This system is compiled without RDMA support." << endl;
			assert(false);
			return 0;
		}

		int RdmaWrite(int dst_tid, int dst_nid, char *local, uint64_t size, uint64_t remote_offset)
		{
			cout << "This system is compiled without RDMA support." << endl;
			assert(false);
			return 0;
		}

		int RdmaWriteSelective(int dst_tid, int dst_nid, char *local, uint64_t size, uint64_t remote_offset)
		{
			cout << "This system is compiled without RDMA support." << endl;
			assert(false);
			return 0;
		}

		int RdmaWriteNonSignal(int dst_tid, int dst_nid, char *local, uint64_t size, uint64_t off)
		{
			cout << "This system is compiled without RDMA support." << endl;
			assert(false);
			return 0;
		}
	};

public:
	RDMA_Device *dev;

	RDMA() : dev(NULL) { }

	~RDMA() { }

	void init_rdma(int num_nodes, int num_threads, int nid, char *mem, uint64_t mem_sz, string ipfn)
	{
		dev = new RDMA_Device(num_nodes, num_threads, nid, mem, mem_sz, ipfn);
	}

	inline static bool has_rdma()
	{
		return false;
	}

	inline static RDMA &get_rdma()
	{
		static RDMA rdma;
		return rdma;
	}
};


#endif

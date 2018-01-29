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

#include <cstring>

class Mem
{
private:
	int num_servers;
	int num_threads;

	// The memory layout: rdma-buffer | ring-buffer
	// The rdma-buffer and ring-buffer are only used when HAS_RDMA
	char *mem;
	uint64_t mem_sz;

	char *buf; // #threads
	uint64_t buf_sz;
	uint64_t buf_off;

	char *rbf; // #thread x #servers
	uint64_t rbf_sz;
	uint64_t rbf_off;

public:
	Mem(int _num_servers, int _num_threads, int _buf_sz_mb, int _rbf_sz_mb)
		: num_servers(_num_servers), num_threads(_num_threads)
	{
		// calculate memory usage & only used by RDMA device

		buf_sz = MiB2B(_buf_sz_mb);
		rbf_sz = MiB2B(_rbf_sz_mb);

		mem_sz = buf_sz*num_threads + rbf_sz * num_servers * num_threads;
		mem = (char*)malloc(mem_sz);
		memset(mem, 0, mem_sz);

		buf_off = 0;
		buf = mem + buf_off;
		rbf_off = buf_off + buf_sz * num_threads;
		rbf = mem + rbf_off;
	}

	~Mem()
	{
		free(mem);
	}

	inline char* memory()
	{
		return mem;
	}
	inline uint64_t memory_size()
	{
		return mem_sz;
	}

	// buffer
	inline char *buffer(int tid)
	{
		return buf + buf_sz * tid;
	}
	inline uint64_t buffer_size()
	{
		return buf_sz;
	}
	inline uint64_t buffer_offset(int tid)
	{
		return buf_off + buf_sz * tid;
	}

	// ring-buffer
	inline char *ring(int tid, int sid)
	{
		return rbf + rbf_sz * num_servers * tid + rbf_sz * sid;
	}
	inline uint64_t ring_size()
	{
		return rbf_sz;
	}
	inline uint64_t ring_offset(int tid, int sid)
	{
		return rbf_off + rbf_sz * num_servers * tid + rbf_sz * sid;
	}

};

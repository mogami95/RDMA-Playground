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

#include "mpi.h"

#include <emmintrin.h>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

#define LOG_I Logger(__FILE__, __LINE__, 0).stream()  //info
#define LOG_W Logger(__FILE__, __LINE__, 1).stream()  //warn
#define LOG_E Logger(__FILE__, __LINE__, 2).stream()  //error

class Logger
{
public:
	Logger() = delete;
	Logger(const char* file, int line, int severity) : file_(file), line_(line), severity_(severity), log_stream_()
	{
		log_stream_<<"["<<file_<<",L:"<<line_<<"] : "; //prefix of cout
	}
	~Logger()
	{
		std::cout<<log_stream_.str();
	}

	inline std::stringstream& stream()
	{
		return log_stream_;
	}

private:
	std::string file_;
	int line_;
	int severity_;
	std::stringstream log_stream_;
};


#define KiB2B(_x)	((_x) * 1024ul)
#define MiB2B(_x)	(KiB2B((_x)) * 1024ul)
#define GiB2B(_x)	(MiB2B((_x)) * 1024ul)

#define B2KiB(_x)	((_x) / 1024.0)
#define B2MiB(_x)	(B2KiB((_x)) / 1024.0)
#define B2GiB(_x)	(B2MiB((_x)) / 1024.0)

const uint64_t kilobyte = 1024;
const uint64_t megabyte = kilobyte*kilobyte;
const uint64_t gigabyte = megabyte*kilobyte;


uint64_t get_usec()
{
	struct timespec tp;
	/* POSIX.1-2008: Applications should use the clock_gettime() function
	   instead of the obsolescent gettimeofday() function. */
	/* NOTE: The clock_gettime() function is only available on Linux.
	   The mach_absolute_time() function is an alternative on OSX. */
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return ((tp.tv_sec * 1000 * 1000) + (tp.tv_nsec / 1000));
}

void cpu_relax(int u)
{
	int t = 166 * u;
	while ((t--) > 0)
		_mm_pause(); // a busy-wait loop
}


int global_tcp_port=22355;
int global_num_threads = 8;
int global_per_thread_buf_size_mb = 500;
int global_per_ring_buf_size_mb = 100;


int _my_rank;
int _num_workers;

inline int get_worker_id()
{
	return _my_rank;
}

inline int get_num_workers()
{
	return _num_workers;
}

void init_worker(int &argc, char** &argv)
{
	int provided = -1;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	if(provided != MPI_THREAD_MULTIPLE)
	{
		printf("MPI do not Support Multiple thread\n");
		exit(-1);
	}
	MPI_Comm_size(MPI_COMM_WORLD, &_num_workers);
	MPI_Comm_rank(MPI_COMM_WORLD, &_my_rank);
}

void worker_finalize()
{
	MPI_Finalize();
}

void worker_barrier()
{
	MPI_Barrier(MPI_COMM_WORLD);
}

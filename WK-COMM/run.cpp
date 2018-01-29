
#define HAS_RDMA

#include "mem.hpp"
#include "rdma.hpp"
#include "rdma_adaptor.hpp"

#include <thread>

using namespace std;


void thread_exec_RdmaWrite(Mem *mem, int tid)
{
	char *buf = mem->buffer(tid);

	char msg[16]="hello,miao_ !";
	msg[11]=char(tid+'0');
	int msg_size=strlen(msg);

	// RdmaWrite
	if(get_worker_id()==get_num_workers()-1)
	{
		//strcpy(buf,msg);
		memset(buf, ('a'+tid), mem->buffer_size());

		RDMA &rdma = RDMA::get_rdma();
		for(int i=0; i<get_num_workers()-1; ++i)
		{
			int dst_tid=tid, dst_nid=i;
			rdma.dev->RdmaWrite(dst_tid, dst_nid, buf, mem->buffer_size(), mem->buffer_offset(dst_tid));
		}

		//LOG_I<<buf<<" ... at rank="<<get_worker_id()<<endl;
		LOG_I<<"front="<<buf[0]<<", back="<<buf[mem->buffer_size()-1]<<" ... at rank="<<get_worker_id()<<endl;
	}
	worker_barrier();
	if(get_worker_id()!=get_num_workers()-1)
	{
		usleep(1000*1000*(tid+1)*0.2);  //fix output-format

		//LOG_I<<buf<<" ... at rank="<<get_worker_id()<<endl;
		LOG_I<<"front="<<buf[0]<<", back="<<buf[mem->buffer_size()-1]<<" ... at rank="<<get_worker_id()<<endl;
	}
}

void thread_exec_RdmaRead(Mem *mem, int tid)
{
	char *buf = mem->buffer(tid);

	char msg[16]="hello,miao_ !";
	msg[11]=char(tid+'0');
	int msg_size=strlen(msg);

	// RdmaRead
	if(get_worker_id()==get_num_workers()-1)
	{
		//strcpy(buf,msg);
		memset(buf, ('a'+tid), mem->buffer_size());

		//LOG_I<<buf<<" ... at rank="<<get_worker_id()<<endl;
		LOG_I<<"front="<<buf[0]<<", back="<<buf[mem->buffer_size()-1]<<" ... at rank="<<get_worker_id()<<endl;
	}
	worker_barrier();
	if(get_worker_id()!=get_num_workers()-1)
	{
		usleep(1000*1000*(tid+1)*0.2);  //fix output-format

		RDMA &rdma = RDMA::get_rdma();
		int dst_tid=tid, dst_nid=get_num_workers()-1;
		rdma.dev->RdmaRead(dst_tid, dst_nid, buf, mem->buffer_size(), mem->buffer_offset(tid));

		//LOG_I<<buf<<" ... at rank="<<get_worker_id()<<endl;
		LOG_I<<"front="<<buf[0]<<", back="<<buf[mem->buffer_size()-1]<<" ... at rank="<<get_worker_id()<<endl;
	}
}

void thread_exec_sendrecv(RDMA_Adaptor *rdma_adaptor, int tid)
{
	int dst_tid=global_num_threads-1-tid;
	//int dst_tid=0;

	int dst_sid=get_num_workers()-1;
	//send msg
	{
		string msg;
		size_t msg_size=6*megabyte*3;
		msg.reserve(msg_size);
		msg.assign("msg_from_wx_ty_!");
		msg[10]=get_worker_id()+'0';
		msg[13]=tid+'0';
		msg.append(msg_size-16, 'a'+tid);

		rdma_adaptor->send(tid, dst_sid, dst_tid, msg);
		rdma_adaptor->send(tid, dst_sid, dst_tid, msg);
	}
	//recv msg
	if(get_worker_id()==dst_sid)
	{
		usleep(1000*1000*tid*0.2);
		//if(tid!=0) return;

		int total_msg_count=get_num_workers()*2;
		//int total_msg_count=get_num_workers()*global_num_threads*2;
		for(int i=0; i<total_msg_count; ++i)
		{
			string msg;
			rdma_adaptor->recv(tid,i%get_num_workers(), msg);
			string str(msg, 0, 20);
			LOG_I<<str<<"..., msg.size="<<double(msg.size())/1024/1024<<"mb, recved by w"<<get_worker_id()<<" t"<<tid<<endl;
			//LOG_I<<msg<<", recved by w"<<get_worker_id()<<" t"<<tid<<endl;
		}
	}
}

void thread_test_sendrecv(RDMA_Adaptor *rdma_adaptor, int tid)
{
	int dst_tid=0;
	int dst_sid;
	//send msg
	for(dst_sid=0;dst_sid<get_num_workers();++dst_sid)
	{
		string msg;
		size_t msg_size=100*megabyte;
		msg.reserve(msg_size);
		msg.assign("msg_from_wx_ty_!");
		msg[10]=get_worker_id()+'0';
		msg[13]=tid+'0';
		msg.append(msg_size-16, 'a'+tid);

		rdma_adaptor->send(tid, dst_sid, dst_tid, msg);
		//rdma_adaptor->send(tid, dst_sid, dst_tid, msg);
	}
	LOG_I<<"MSG SENDING DONE at rank="<<get_worker_id()<<endl;
	//recv msg
	for(dst_sid=0;dst_sid<get_num_workers();++dst_sid)
	{
		string msg;
		rdma_adaptor->recv(tid, dst_sid, msg);
		//rdma_adaptor->recv(tid, dst_sid, msg);
		string str(msg, 0, 20);
		LOG_I<<str<<"..., msg.size="<<double(msg.size())/1024/1024<<"mb, recved by w"<<get_worker_id()<<" t"<<tid<<endl;
	}
}

int main(int argc, char** argv)
{
	init_worker(argc, argv);

	string host_fname = std::string(argv[1]);  // specify the path of MPI & IB-NIC hosts file
	global_tcp_port=22355;
	//global_num_threads = 8;
	//global_per_thread_buf_size_mb = 100;
	///global_per_ring_buf_size_mb = 300;
	global_num_threads = 1;
	global_per_thread_buf_size_mb = 1024;
	global_per_ring_buf_size_mb = 1024;
	/*
	if(argc>=3)
		global_tcp_port = atoi(argv[2]);
	if(argc>=4)
		global_num_threads = atoi(argv[3]);
	if(argc>=5)
		global_per_thread_buf_size_mb = atoi(argv[4]);
	if(argc>=6)
		global_per_ring_buf_size_mb = atoi(argv[5]);
	*/

	uint64_t t = get_usec();
	Mem *mem = new Mem(get_num_workers(), global_num_threads, global_per_thread_buf_size_mb, global_per_ring_buf_size_mb);
	RDMA::get_rdma().init_rdma(get_num_workers(), global_num_threads, get_worker_id(), mem->memory(), mem->memory_size(), host_fname, global_tcp_port);
	RDMA_Adaptor *rdma_adaptor = new RDMA_Adaptor(get_num_workers(), global_num_threads, get_worker_id(), mem);
	t = get_usec() - t;
	LOG_I << "INFO: Initializing RDMA Done! (" << t / 1000  << " ms) at workerID="<< get_worker_id() << endl;
	worker_barrier();


	thread my_threads[global_num_threads];
	for (int tid = 0; tid < global_num_threads; ++tid)
	{
		//my_threads[tid]=thread(thread_exec_RdmaRead, std::ref(mem), tid);
		//my_threads[tid]=thread(thread_exec_RdmaWrite, std::ref(mem), tid);
		my_threads[tid]=thread(thread_test_sendrecv, rdma_adaptor, tid);
	}


	for (int tid = 0; tid < global_num_threads; ++tid)
	{
		if(my_threads[tid].joinable()) my_threads[tid].join();
	}
	delete mem;
	delete rdma_adaptor;
	worker_finalize();
	return 0;
}

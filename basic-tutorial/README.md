## Usage

    make -j

============rdma_demo_1side============

In one server (e.g. worker2):  
    ./rdma_demo_1side $READER_IP_ADDR writer

In another server (e.g. worker5):  
    ./rdma_demo_1side $WRITER_IP_ADDR reader

============rdma_demo_2sides============

In one server (e.g. worker2):  
    ./rdma_demo_2sides $READER_IP_ADDR writer

In another server (e.g. worker5):  
    ./rdma_demo_2sides $WRITER_IP_ADDR reader

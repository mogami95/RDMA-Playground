## Usage

    make -j

============rdma_test============

In one server (e.g. worker2):
    ./rdma_test ib5 writer

In another server (e.g. worker5)
    ./rdma_test ib2 reader

============benchmark============

In one server (e.g. worker2):
    ./benchmark ib5 writer

In another server (e.g. worker5)
    ./benchmark ib2 reader

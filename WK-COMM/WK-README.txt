cd /data/miao/RDMA-Playground/WK-COMM/
mpiexec -n <#servers> -f conf_ib ./run /home/miao/conf_ib

Note:
the 1st "conf_ib" file is used to launch MPI among all selected machines (this file can be placed in master's local dir or any nfs-dirs which can be accessed by master)
the 2nd "conf_ib" file is located in the local(non-nfs) dir of every machine
please ensure the 1st "conf_ib" and the 2nd "conf_ib" contain the same content and sync them all after making chances (must!)

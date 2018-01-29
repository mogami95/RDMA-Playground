# Rdma playground

This is repo is used to play around Rdma...

Mainly follows:

[infiniband...] (https://blog.zhaw.ch/icclab/infiniband-an-introduction-simple-ib-verbs-program-with-rdma-write/).

[the Drtm Rdma module] (https://github.com/SJTU-IPADS/drtm/tree/master/memstore),

[Rdma user manual] (http://www.mellanox.com/related-docs/prod_software/RDMA_Aware_Programming_user_manual.pdf), 

[RDMAMojo blog] (http://www.rdmamojo.com/2013/01/26/ibv_post_send/).


What is the RDMA (introductions & concepts)
http://digitalvampire.org/blog/index.php/2006/07/13/what-is-this-thing-called-rdma/
http://searchstorage.techtarget.com/definition/Remote-Direct-Memory-Access
https://htor.inf.ethz.ch/blog/index.php/2016/05/15/what-are-the-real-differences-between-rdma-infiniband-rma-and-pgas/

RDMA (IBverb/RdmaCM) Programming Tutorials
http://fabbritech.blogspot.hk/2012/07/linux-rdma-verbs-api-tutorials.html
https://thegeekinthecorner.wordpress.com/category/infiniband-verbs-rdma/

Slides & Handbook
http://www.cs.unh.edu/~rdr/rdma-intro-module.ppt
https://cw.infinibandta.org/document/dl/7268  ( Introduction to InfiniBand for End Users: Chapter 1, 4, 5 )


About libibverbs: 
libibverbs is a low-level layer that allows one to use RDMA in his code, it uses RDMA technology primitives that were defined in the InfiniBand specifications
Roland Dreier started the development of this library, but now it is maintained by other people.
And librdmacm is an attempt to make the RDMA programming closer to socket programming.
https://github.com/jcxue/RDMA-Tutorial/wiki
https://zcopy.wordpress.com/2010/10/08/quick-concepts-part-1-%E2%80%93-introduction-to-rdma/
http://forum.huawei.com/enterprise/zh/thread-406491.html
https://blog.zhaw.ch/icclab/infiniband-an-introduction-simple-ib-verbs-program-with-rdma-write/
http://www.nminoru.jp/~nminoru/network/infiniband/ibverbs-getstarted.html


Software:
(0)
    https://github.com/MiaoLoud/Rdma_Playground (improved the makefile compared with Yuzhen’s)
(1)
    https://www.openfabrics.org/downloads/verbs
    https://www.openfabrics.org/downloads/rdmacm
(2)
    http://ipads.se.sjtu.edu.cn/wukong/librdma-1.0.0.tar.gz


TIPS: 
Do remember that modify <memlock> in “/etc/security/limits.conf” file, otherwise you may only execute RDMA program with registered RDMA MEM SIZE less than 64KB
http://www.rdmamojo.com/2012/09/07/ibv_reg_mr/
http://blog.51cto.com/kerry/300784
https://askubuntu.com/questions/57915/environment-variables-when-run-with-sudo

Please do remember switch your NIC (network interface card) to IB-NIC when executing RDMA-based programs!!!
(ETH-NIC sometimes can also execute RDMA-based programs with a lower preformance) 

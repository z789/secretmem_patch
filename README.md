secretmem

	从kernel v5.14.7 移植 secretmem功能。
 
	Ubuntu: kernel v5.4.0.89 

	CentOS8: kernel v4.18.0-305.10.2.el8


什么是secretmem：

       secretmem是kernel v5.14.7 加入的一个申请 "secret memory"的功能，应用程序通过新引入的系统调用memfd_secret使用该功能。
        
       "secret memory"的目标： 1. 除了程序本身可以访问该内存，其他应用都不可以。

                               2. 内核也不可以。

       使用场景： 存储敏感数据，比如 密钥等。

       参考：https://lwn.net/Articles/865256/


secretmem 存储敏感数据相比较使用tpm、sgx、amd SME的优点：

        1. 不需要额外的硬件、不依赖硬件的特性、不需要增加驱动、不需要单独的支撑库。

        2. 存储容量大，存储速度快。

        3. 使用简单。

        缺点： 理论上，安全性没有使用硬件上高。但配合系统的其他安全功能, 可是满足很多使用场景的需求。


向底版本内核移植的改变：

        1. secretmem在 kernel v5.14.7 中是引入一个新系统调用。在 v5.4.0.89 和 v4.18.0-305.10.2.el8中是为 memfd_create增加一个参数MFD_SECRET实现。

        2. 更低的版本，例如 3.10.0（centos7），由于内核代码变化大，需要修改的多，可能移植失败。


secretmem的现状：

        1. 从提出到引入 kernel v5.14.7， 变化了很多。将来还会有fix、变化。

        2. 从测试来看，实现了第一个目标，就满足了大多数的使用场景（例如：dump 敏感数据的内存）。

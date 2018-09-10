这是关于fastcgi的相关配置   
一般配置nginx使用

编译方式：
	gcc fastcgi.c -o fasttest -L /home/xxh/local/lib/ -lfcgi -I /home/xxh/local/include/
	gcc cmccMysql.c logLog.c fastcgi.c -o fasttest -L /home/xxh/local/lib/ -lfcgi -lcjson -L /usr/lib/x86_64-linux-gnu/ -lmysqlclient -I /home/xxh/local/include/ -I /usr/include/mysql/ -I /home/xxh/local/include/cjson/

运行方式：
	spawn-fcgi -a 127.0.0.1 -p 8010 -f ./fasttest


各种安装包
need mysqlclient
	sudo apt-get install libmysqlclient-dev   ubuntu下

cjson
	cJSON-master.zip  
	安装方式：
		mkdir build
		cd build
		cmake .. -DENABLE_CJSON_UTILS=On -DENABLE_CJSON_TEST=Off -DCMAKE_INSTALL_PREFIX=/home/xxh/local
		make
		make install	


fcgi-2.4.1-SNAP-0910052249.tar.gz 
	
	./configure
	make
	make install

	若make出错 关于EOF
	可在 include/fcgio.h  
    	 加  #include <cstdio>

 spawn-fcgi-1.6.4.tar.gz
	./configure
  	make
  	make install


每个安装包可以指定的位置


mysql 表创建
库名xxhdatabase
create table userdatahttp (name varchar(100),passdata varchar(64),id varchar(32) primary key NOT NULL,type int DEFAULT 0);

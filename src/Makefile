gcc fastcgi.c cmccMysql.c logLog.c -o fasttest -L /home/xxh/local/lib/ -lcjson -lfcgi -lmysqlclient -I /home/xxh/local/include/ -I ./
spawn-fcgi -a 127.0.0.1 -p 8010 -f ./fasttest


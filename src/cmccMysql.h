#ifndef _CMCCMYSQL_H
#define _CMCCMYSQL_H
#include "type.h"
int mysqlLogin(mysqldata *,logdata *);
int mysqlReLogin(mysqldata *,logdata *);
int mysqlSearch(mysqldata *,char const *);//返回0 表示正常且有数据 1为正常但没数据  <0表示出错
int coluNum(char const *,void *,int *);//找出所要字段在表中列数
char **fetchDate(void *);//传入为 MYSQL_RES 型指针
int mysqlFreeRes(void *,logdata *);//传入为 MYSQL_RES 型指针

int mysqlClose(void *);//传入 MYSQL 类型指针
#endif

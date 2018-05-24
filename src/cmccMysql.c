#include <stdio.h>
#include <string.h>
#include "cmccMysql.h"
#include "mysql.h"

int mysqlLogin(mysqldata *dat,logdata *log)
{
    if(dat == NULL)
    {
        return TCS_ARG_NULL;
    }
	

    dat->handle = mysql_init(NULL);
    if(dat ->handle == NULL)
    {
        return TCS_OTHER_ERR - 1;
    }
	 logLog(log,LOGWAR,"mysql_init is ok");
    dat->handle = mysql_real_connect(dat->handle,
                                     dat->host.date,
                                     dat->user.date,
                                     dat->passwd.date,
                                     dat->db.date,
                                     dat->port,NULL,0);
    if(dat->handle ==NULL)
    {
        return TCS_OTHER_ERR -2;
    }
	 logLog(log,LOGWAR,"mysql_real_connect is ok");

    return 0;

}
int mysqlReLogin(mysqldata *dat,logdata *log)
{
    if(dat ==NULL)
    {
        return TCS_ARG_NULL;
    }
	 logLog(log,LOGWAR,"relog start is ok");
    if(dat->resData)
    {
        mysqlFreeRes(dat->resData,log);
        dat->resData =NULL;
    }
	 logLog(log,LOGWAR,"relog mysqlFreeRes is ok");
    if(dat->handle)
    {
        mysqlClose(dat->handle);
        dat->handle = NULL;
    }
	 logLog(log,LOGWAR,"relog mysqlclose is ok");
    int ret=0;
    ret = mysqlLogin(dat,log);
    
	 logLog(log,LOGWAR,"relog  is ok");
    return ret;
}

int mysqlSearch(mysqldata *dat,char const *order)
{
    if(dat ==NULL)
    {
        return TCS_ARG_NULL;
    }
    char tt[1024]={0};
    if(order != NULL)
    {//表示执行自定义查询命令
        strncpy(tt,order,strlen(order));
    }
    else if(dat ->tablename.date != NULL)
    {//查表全部内容
        sprintf(tt,"%s %s","select * from",dat->tablename.date);

    }
    else 
    {
        return TCS_ARG_NULL;
    }
    

    if(mysql_query(dat->handle,tt))
    {
        //出错 
        return TCS_OTHER_ERR -3;
    }

    MYSQL_RES *restem = mysql_store_result(dat->handle);
    if(restem == NULL)
    {
        return TCS_OTHER_ERR -4;
    }
    dat->resData = restem;

    int num = (int)mysql_num_rows(restem);

    if(num ==0)
    {
        return TCS_DATA_NULL;//表示没有数据
    }

    return 0;

}

int coluNum(char const *dat,void *dd,int *res)
{//
    if(dat ==NULL || dd ==NULL || res ==NULL)
    {
        return TCS_ARG_NULL;
    }
    MYSQL_RES *mys = (MYSQL_RES*)dd;
    unsigned int num = mysql_num_fields(mys);
    MYSQL_FIELD *fileds =  mysql_fetch_fields(mys);
    unsigned int i=0;
    for(;i<num;++i)
    {
        if(strncmp(dat,fileds[i].name,strlen(dat))==0)
        {
            *res =(int)i;
            return 0;
        }
    }

    return TCS_DATA_NULL;

}

char **fetchDate(void *dd)
{
    if(dd ==NULL)
    {
        return NULL;
    }

    MYSQL_RES *mys = (MYSQL_RES*)dd;
    return (char**)mysql_fetch_row(mys);
}

int mysqlFreeRes(void *dd,logdata *log)
{
    if(dd ==NULL)
    {
        return TCS_ARG_NULL;
    }
	 logLog(log,LOGWAR,"mysqlFreeRes start is ok");
    MYSQL_RES *res =(MYSQL_RES*)dd;
    mysql_free_result(res);
	 logLog(log,LOGWAR,"mysqlFreeRes mysql_free_result is ok");
    return 0;

}
int mysqlClose(void *tt)
{
    if(tt ==NULL)
    {
        return TCS_ARG_NULL;
    }
    MYSQL *res = (MYSQL*) tt;
    mysql_close(res);
    return 0;
}

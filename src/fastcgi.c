#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
#include "type.h"
#include "cmccMysql.h"
#include "fcgi_config.h"
#include "logLog.h"
//#include "util_cgi.h"
#include "fcgi_stdio.h"

#define SPACENUM 1024


int infget(const char *dat,const char aa,const char bb,const char *key,char *value,int star)
{	//key aa value bb...  =dat  star is where to check dat string
	if(dat ==NULL || key ==NULL || value ==NULL || aa ==0)
	{
		return -1;
	}
	int len = strlen(dat);
	int keylen = strlen(key);
	int ret =0;
	if(star>len)
	{
		return -2;
	}
	if(star<0)
	{
		if(star + len <0)
		{
			return -3;
		}
		star +=len;
	}
	char *chatem =strstr(dat+star,key);
	if(chatem ==NULL)
	{
		return -4; 
	}
	star = (int)(chatem - dat);
	star += keylen;
	while(*(dat+star)!=0)
	{
		if(*(dat + star)==' ')
		{
			++star;
			continue;
		}
		if(*(dat + star)== aa)
		{
			if(*(dat + star + 1)==aa)
			{
				++star;
				continue;
			}
			++star;
			break;
		}
		return 0;
	}
	if(*(dat+star)==0)
	{return 0;}
	while(*(dat+star)!=0)
	{
		if(*(dat + star)==' ')
		{
			++star;
			continue;
		}
		break;
	}
	if(*(dat+star)==0)
	{return 0;}
	ret=0;
	while(*(dat + star)!=0 && *(dat + star)!=bb)
	{
		*(value + ret) = *(dat + star);
		++star;	
		++ret;
	}
	return star;	
}


int main(int argc, char *argv[])
{
	int count = 0;
	int len=0;
	int ret=0;
	logdata *log =NULL;
	char *host="127.0.0.1";
	char *usr="xxh";
	char *passwd="xxh123456";
	char *dbdata="xxhdatabase";
	char *tablename="userdatahttp";
	char *temspace=NULL;
	mysqldata sqldata;
	memset(&sqldata,0,sizeof(mysqldata));
	sqldata.port=3306;
	sqldata.host.date=host;
	sqldata.host.len=strlen(host);	
	sqldata.user.date = usr;
	sqldata.user.len = strlen(usr);
	sqldata.passwd.date = passwd;
	sqldata.passwd.len = strlen(passwd);
	sqldata.db.date = dbdata;
	sqldata.db.len = strlen(dbdata);
	sqldata.tablename.date = tablename;
	sqldata.tablename.len = strlen(tablename);

	log = logInit("./fcgilog.log",10);//int 为Kb	
	if(log == NULL)
	{	
		printf("log is NULL\n");
		exit(-1);
	}
	logLog(log,LOGINF,"-----------------------------------------------");

	temspace = malloc(sizeof(char) * SPACENUM);
	if(temspace ==NULL)
	{
		logLog(log,LOGERR,"malloc is NULL");
		exit(-1);

	}

		ret = mysqlLogin(&sqldata,log);
		if(ret !=0)
		{
			logLog(log,LOGERR,"mysqlLogin is err %d",ret);
			exit(-1);
		}


		int i=0;

		while (FCGI_Accept() >= 0) 
		{
			char *contentLength = getenv("CONTENT_LENGTH");
			if (contentLength != NULL) {
				len = strtol(contentLength, NULL, 10);
			}
			else {
				len = 0;
			}
			if (len <= 0) {
				logLog(log,LOGWAR,"CONTENT_LENGTH less 0");
				continue;
			}
			else {
				memset(temspace,0,sizeof(char) * SPACENUM);
				for (i = 0; i < len; i++) {
					*(temspace+i) = getchar();
				}
			}
			logLog(log,LOGINF,"http body %s",temspace);

			char namedata[64]={0};
			char pass[64] ={0};
			ret = infget(temspace,'=','&',"username",namedata,0);
			logLog(log,LOGINF,"username %d %s",ret,namedata);
			ret = infget(temspace,'=','&',"password",pass,0);
			logLog(log,LOGINF,"password %d %s",ret,pass);

			char seachdata[1024]={0};
			sprintf(seachdata,"%s%s%s%s%s%s%s","select id,type from ",sqldata.tablename.date," where name =\'",namedata,"\' and passdata = \'",pass,"\'");
			logLog(log,LOGINF,"%s",seachdata);
			ret =  mysqlSearch(&sqldata,seachdata);
			cJSON *json = cJSON_CreateObject();
			if(ret <0)
			{
				
				logLog(log,LOGERR,"mysqlSearch err %d",ret);
				logLog(log,LOGWAR,"start to relogin mysql");
				ret = mysqlReLogin(&sqldata,log);	
				if(ret !=0)
				{
					logLog(log,LOGERR,"mysqlSearch relogin err %d",ret);
					
				//	cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(1));
				//	cJSON_AddItemToObject(json,"msg",cJSON_CreateString("system err"));
				}
				else
				{
					logLog(log,LOGINF,"mysqlSearch relogin ok");
					
					ret =  mysqlSearch(&sqldata,seachdata);
					
				}
			
			}
			if(ret ==0)
			{
				cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(0));
				cJSON_AddItemToObject(json,"msg",cJSON_CreateString("ok"));
				logLog(log,LOGINF,"%s %s ok",namedata,pass);

			}
			else if(ret ==1)
			{
				cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(1));
				cJSON_AddItemToObject(json,"msg",cJSON_CreateString("用户名或密码错误"));
				logLog(log,LOGWAR,"%s %s not fit",namedata,pass);

			}
			else
			{	
				cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(1));
				cJSON_AddItemToObject(json,"msg",cJSON_CreateString("system err"));
				logLog(log,LOGERR,"mysqlSearch seach err %d",ret);
			}

			//		cJSON_AddItemToObject(json,"status",cJSON_CreateNumber(0));
			//		cJSON_AddItemToObject(json,"msg",cJSON_CreateString("okok"));
			cJSON *datajson = cJSON_CreateObject();
			cJSON_AddItemToObject(datajson,"id",cJSON_CreateNumber(123));
			cJSON_AddItemToObject(json,"data",datajson);
			char *buf = cJSON_Print(json);
			int bufnum = strlen(buf);




			/************************发送区************************************/

			printf("Content-type: application/json\r\n");
			printf("Content-Length: %d\r\n",bufnum);
			printf("\r\n");

			printf("%s",buf);


			mysqlFreeRes(sqldata.resData,log);
			cJSON_free(buf);
			cJSON_Delete(json);



			/*
			   printf("Content-type: text/html\r\n");
			   printf("\r\n");

			   printf("<title>Fast CGI Hello!</title>");
			   printf("<h1>Fast CGI Hello!</h1>");
			   printf("Request number %d running on host <i>%s</i>\n", ++count, getenv("SERVER_NAME"));
			   printf("remote addr %s\n", getenv("REMOTE_ADDR"));
			   printf("<br/>");
			   char *contentLength = getenv("CONTENT_LENGTH");
			   if (contentLength != NULL) {
			   len = strtol(contentLength, NULL, 10);
			   }
			   else {
			   len = 0;
			   }
			   if (len <= 0) {
			   printf("No data from standard input.<p>\n");
			   }
			   else {
			   int i;
			   printf("Standard input:<br>\n<pre>\n");
			   for (i = 0; i < len; i++) {
			   printf("%c",getchar());
			   }
			   }

			 */



			//		printf("{");
			//		printf("status:1,");
			//		printf("msg:\"aaa\",");
			//		printf("data:{}");
			//		printf("}");

			//		printf("{");
			//		printf("\"status\":0,");
			//		printf("\"msg\":\"ok\"");
			//printf("\"data\":{}");
			//		printf("}");

			/*
			   {
status:0, 0表示正常 1表示错误
msg:'', 状态的说明
data:{},此接口不需要数据
}

			 */	

			}
	logLog(log,LOGINF,"------------------------end-----------------------");

return 0;
}
/*
   printf("Content-type: text/html\r\n");
   printf("\r\n");

   printf("<title>Fast CGI Hello!</title>");
   printf("<h1>Fast CGI Hello!</h1>");
   printf("Request number %d running on host <i>%s</i>\n", ++count, getenv("SERVER_NAME"));
   printf("remote addr %s, remote port %s server addr %s, server port %s\n", getenv("REMOTE_ADDR"),
   getenv("REMOTE_PORT"),
   getenv("SERVER_ADDR"),
   getenv("SERVER_PORT"));
   printf("<br/>");
   char *contentLength = getenv("CONTENT_LENGTH");
   if (contentLength != NULL) {
   len = strtol(contentLength, NULL, 10);
   }
   else {
   len = 0;
   }
   if (len <= 0) {
   printf("No data from standard input.<p>\n");
   }
   else {
   int i;
   printf("Standard input:<br>\n<pre>\n");
   for (i = 0; i < len; i++) {
   printf("%c",getchar());
   }
   }
 */

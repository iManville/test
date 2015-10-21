
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSERT_SQL "INSERT INTO t_file VALUES (null,?)"

static int size;
static char *buf;

static int insert_file(MYSQL*);

int main(int argc, char *argv[]){

	MYSQL *conn;
	
	int sts;
	FILE *pf;
	size_t result; //返回值是读取的内容数量

	pf = fopen("/home/ec2-user/workspace/test/README.md","rb");
	if (pf == NULL) {
		printf(" Open file error\n");
		exit(0);
	}

	// 获取文件大小
	fseek(pf,0,SEEK_END); //指针移到文件末尾
	size = (int)ftell(pf); //获取长度
	rewind(pf); // 函数rewind()把文件指针移到由stream(流)指定的开始处, 同时清除和流相关的错误和EOF标记  

	buf = (char*)malloc(sizeof(char)*size);
	result = fread(buf,1,size,pf);
	if (result != size) {
		printf(" Read file error\n");
		exit(0);
	}

	printf("size:%d\n",size);

	//初始化
	conn = mysql_init(NULL);
	if (conn == NULL) {
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(0);
	}
	//连接Mysql
	//if (mysql_real_connect(conn, "localhost", "user", "pass", "database", 0, NULL, 0) == NULL) { 
	if (mysql_real_connect(conn, "127.0.0.1", "root", "123456", "test", 0, NULL, 0) == NULL) {
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		exit(0);
	}

	sts = insert_file(conn);
	if (sts) {
		printf(" Insert file error\n");
		exit(0);
	}

	//释放资源	
	mysql_close(conn); 
	
	fclose(pf);
	free(buf);

	return 0;
}

/*
 * 插入文件
 */
int insert_file(MYSQL *conn) {
	
	MYSQL_STMT *stmt;
	MYSQL_BIND bind[1];
	
	//SQL状态初始化
	stmt = mysql_stmt_init(conn);
	if (!stmt) {
		fprintf(stderr, " mysql_stmt_init(), out of memory\n"); 
		exit(0);
	}
	//预处理
	if (mysql_stmt_prepare(stmt, INSERT_SQL, strlen(INSERT_SQL))) {
		fprintf(stderr, " mysql_stmt_prepare(), INSERT failed: %s\n", mysql_stmt_error(stmt));
		return -1;
	}

	//MEDIUMBLOB PARAM
	bind[0].buffer_type = MYSQL_TYPE_LONG_BLOB;
	//bind[0].buffer_type = MYSQL_TYPE_MEDIUM_BLOB;
	bind[0].buffer = buf;
	bind[0].buffer_length = size;
	bind[0].is_null = 0;
	bind[0].length = 0;

	//解析
	if (mysql_stmt_bind_param(stmt, bind)){
		fprintf(stderr, " mysql_stmt_bind_param() failed: %s\n", mysql_stmt_error(stmt));
		return -1;
	}
	//执行
	if (mysql_stmt_execute(stmt)) {
		fprintf(stderr, " mysql_stmt_execute(), 1 failed: %s\n", mysql_stmt_error(stmt));
		return -1;
	}
	//关闭
	if (mysql_stmt_close(stmt)) {
		fprintf(stderr, " mysql_stmt_close(), 1 failed: %s\n", mysql_stmt_error(stmt));
		return -1;
	}

	return 0;
}


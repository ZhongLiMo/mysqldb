# mysqldb
提供简易c++连接、操作mysql数据库的接口，个人学习mysql使用过程中写的，主要是自己在学习C++和mysql的过程中自己写着玩的。

### 使用时需要注意
##### 1、默认使用bigint(64)作为主键类型，主键名为"id"，这两个规则最好不要修改
    分别由`typedef long long Key`、和'const char* g_strKey = "id"'控制。
    
##### 2、VS这边默认用的是GBK编码，可以修改，由`const char* g_encoding = "GBK"`控制
    在mysql控制台登录后输入查询语句`Show variables like 'character%';`，
    `character_set_client`客户端的字符集，`character_set_results`结果字符集，`character_set_connection`连接字符集，
    这三个系统参数的作用：信息输入路径`client--connection--server`（数据传送方向从左到右），信息输出路径`server--connection--results`（数据传送方向从左到右）。
    
### 使用方法
##### 1、定义全局变量`char tableName[] = "test_user"`作为表名，必须是全局的
##### 2、定义数据表字段的枚举类型，注意要定义一个MAX在最后，因为这个MAX的值等于字段的个数
```
enum TEST_USER
{
	TEST_USER_ID,
	TEST_USER_NAME,
	TEST_USER_NICKNAME,
	TEST_USER_SEX,
	TEST_USER_MAX
};
```
##### 3、设置数据库名、数据库服务器IP端口、用户名密码后调用`DBHandle->Connect(host, user, pswd, database, port)`连接数据库
##### 4、使用类模板生成需要的类
```
typedef DBRecord<TEST_USER, TEST_USER_MAX, tableName> UserRecord;	//表中一行数据
DBTble<UserRecord> userTable;						//完整的表
```
##### 5、调用`DBHandle->Select(userTable, tableName)`即可加载完成整个表`userTable`
##### 6、更多的增删改查，可以看`test.cpp`中举的例子。

如果有问题、BUG还望指出：965757151@qq.com

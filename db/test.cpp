#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <iostream>
#include "mysqldb.hpp"
using namespace std;

char tableName[] = "test_user";

enum TEST_USER
{
	TEST_USER_ID,
	TEST_USER_NAME,
	TEST_USER_NICKNAME,
	TEST_USER_SEX,
	TEST_USER_MAX,
};

int main()
{
	const char user[] = "root";
	const char pswd[] = "123456";
	const char host[] = "localhost";
	const char database[] = "test";
	{
		DBHandle->Connect(host, user, pswd, database);
		typedef DATABASE::Record<TEST_USER, TEST_USER_MAX, tableName> UserRecord;
		DATABASE::RecordTable<UserRecord> userTable;

		//select
		std::cout << "after select" << std::endl;
		DBHandle->Select(userTable, tableName);
		auto ite = userTable.begin();
		for (; ite != userTable.end(); ++ite)
		{
			for (unsigned int i = 0; i < (ite->second)->Size(); ++i)
			{
				std::cout << (*ite->second)[i] << "\t";
			}
			std::cout << std::endl;
		}

		//update
		if (userTable.begin() != userTable.end())
		{
			auto userRecord = userTable.begin()->second;
			userRecord->SetString(TEST_USER_NICKNAME, "Jack");
			userRecord->Update();
		}
		std::cout << std::endl << "after update" << std::endl; ite = userTable.begin();
		for (; ite != userTable.end(); ++ite)
		{
			for (unsigned int i = 0; i < (ite->second)->Size(); ++i)
			{
				std::cout << (*ite->second)[i] << "\t";
			}
			std::cout << std::endl;
		}

		//delete
		Key key = 0;
		if (userTable.begin() != userTable.end())
		{
			auto userRecord = userTable.begin()->second;
			key = userRecord->GetKey();
			userTable.DeleteRecord(key);
		}
		std::cout << std::endl << "after delete" << std::endl; ite = userTable.begin();
		for (; ite != userTable.end(); ++ite)
		{
			for (unsigned int i = 0; i < (ite->second)->Size(); ++i)
			{
				std::cout << (*ite->second)[i] << "\t";
			}
			std::cout << std::endl;
		}

		//insert
		if (key)
		{
			auto userRecord = UserRecord::CreateNew(key);
			userRecord->SetString(TEST_USER_NAME, "Libai");
			userRecord->SetString(TEST_USER_NICKNAME, "XiaoBai");
			userRecord->SetInt(TEST_USER_SEX, 1);
			userTable.InsertRecord(userRecord);
		}
		std::cout << std::endl << "after insert" << std::endl; ite = userTable.begin();
		for (; ite != userTable.end(); ++ite)
		{
			for (unsigned int i = 0; i < (ite->second)->Size(); ++i)
			{
				std::cout << (*ite->second)[i] << "\t";
			}
			std::cout << std::endl;
		}

		DBHandle->Close();
	}

	_CrtDumpMemoryLeaks();
	return 0;
}
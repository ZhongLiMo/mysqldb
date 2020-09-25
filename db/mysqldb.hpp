/*
 *	file		mysqldb.hpp
 *	author		lzg
 *	date		2020.09.24
 *	brief		mysql
 *	notice		Datatable primary key name id, type bigint(64)
 *	vertion		1.0
 */

#ifndef MYSQLDB_H
#define MYSQLDB_H


#include <string>
#include <memory>
#include <cassert>
#include <unordered_map>

#include <mysql.h>

/*
 *	notice		Datatable primary key name id, type bigint(64)
 */
typedef long long Key;
const char* g_strKey = "id";
const char* g_encoding = "GBK";
const unsigned int SQL_SIZE = 1024;

namespace DATABASE
{
	class Field
	{
	public:
		operator short();
		operator int();
		operator long();
		operator long long();
		operator float();
		operator double();
		operator std::string();
		friend std::ostream& operator<<(std::ostream& os, const Field& field);
		static void Sprintfs(const Field& field, int& len, char(&strsql)[SQL_SIZE]);
	private:
		void GetField(enum_field_types fieldType, const char* val);
	private:
		Field() {}
		Field(const Field&) = default;
		Field& operator=(const Field&) = default;
	private:
		union
		{
			int                 m_iVal;
			long                m_lVal;
			long long           m_llVal;
			float               m_fVal;
			double              m_dVal;
		};
		bool                    m_bDirty;
		std::string             m_strVal;
		enum_field_types        m_fieldType;
		template<typename Index, Index size, const char* tableName>
		friend class Record;
	};
	template<typename Index, Index size, const char* tableName>
	class Record
	{
	public:
		Key GetKey();
		int GetInt(Index index) const;
		void SetInt(Index index, int val);
		long GetLong(Index index) const;
		void SetLong(Index index, long val);
		long long GetLonglong(Index index) const;
		void SetLonglong(Index index, long long val);
		float GetFloat(Index index) const;
		void SetFloat(Index index, float val);
		double GetDouble(Index index) const;
		void SetDouble(Index index, double val);
		std::string GetString(Index index) const;
		void SetString(Index index, const std::string& val);
	public:
		bool Update();
		size_t Size() const;
		const Field& operator[](unsigned int index) const;
		static void InitDefault(const MYSQL_FIELD* mysqlField, unsigned int fieldsNum);
		static std::shared_ptr<Record> CreateNew(Key key) { return std::shared_ptr<Record>(new Record(key)); }
	private:
		bool Insert();
		bool Delete();
		void SetKey(Key key);
		void GetFields(const MYSQL_FIELD* mysqlField, const MYSQL_ROW& mysqlRow, unsigned int fieldsNum);
	private:
		Record() {}
		Record(Key key);
		Record(const Record&) = delete;
		Record& operator=(const Record&) = delete;
		static std::shared_ptr<Record> CreateNew() { return std::shared_ptr<Record>(new Record()); }
	private:
		Field m_fieldArr[size];
		template<typename RecordType>
		friend class RecordTable;
	private:
		static bool s_default;
		static Record s_defaultRecord;
		static unsigned int s_keyIndex;
		static std::string s_fieldsName[size];
	};
	template<typename RecordType>
	class RecordTable
	{
	public:
		RecordTable() = default;
		virtual ~RecordTable() = default;
		size_t Size() const;
		bool DeleteRecord(Key key);
		bool InsertRecord(std::shared_ptr<RecordType>record);
		void CreateTable(MYSQL_RES* mysqlRes, const MYSQL_FIELD* mysqlField, MYSQL_ROW& mysqlRow, unsigned int fieldsNum, unsigned __int64 rowsNum);
	public:
		typename std::unordered_map<Key, std::shared_ptr<RecordType>>::iterator find(Key key);
		typename std::unordered_map<Key, std::shared_ptr<RecordType>>::iterator begin();
		typename std::unordered_map<Key, std::shared_ptr<RecordType>>::iterator end();
	private:
		RecordTable(const RecordTable&) = delete;
		RecordTable& operator=(const RecordTable&) = delete;
		std::unordered_map<Key, std::shared_ptr<RecordType>> m_recordTable;
	};
	class MysqlDB
	{
	public:
		static MysqlDB* GetMysqlHandle()
		{
			static MysqlDB dataBase;
			return &dataBase;
		}
		bool Query(const char(&strsql)[SQL_SIZE]) const;
		template<typename RecordType>
		void InitDefaultRecord(const char(&strsql)[SQL_SIZE], const RecordType& recordType);
		template<typename RecordType>
		bool Select(RecordTable<RecordType>& recordTable, const char* tableName, const char* order = NULL);
		bool Connect(const char* host, const char* user, const char* passwd, const char* dbname, unsigned int port = MYSQL_PORT, const char* unixSocket = NULL, unsigned long clientFlag = 0);
		void Close() { this->~MysqlDB(); }
	private:
		MysqlDB() : m_mysql(NULL), m_mysqlRes(NULL), m_mysqlRow(NULL), m_mysqlField(NULL) {}
		virtual ~MysqlDB() { if (m_mysql) mysql_close(m_mysql); m_mysql = NULL; }
		MysqlDB(const MysqlDB&) = delete;
		MysqlDB& operator=(const MysqlDB&) = delete;
	private:
		MYSQL*          m_mysql;
		MYSQL_RES*      m_mysqlRes;
		MYSQL_ROW       m_mysqlRow;
		MYSQL_FIELD*    m_mysqlField;
	};

	Field::operator short()
	{
		if (MYSQL_TYPE_TINY == m_fieldType || MYSQL_TYPE_SHORT == m_fieldType || MYSQL_TYPE_LONG == m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldType)
			return m_iVal;
		return -1;
	}
	Field::operator int()
	{
		if (MYSQL_TYPE_TINY == m_fieldType || MYSQL_TYPE_SHORT == m_fieldType || MYSQL_TYPE_LONG == m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldType)
			return m_iVal;
		return -1;
	}
	Field::operator long()
	{
		if (MYSQL_TYPE_TINY == m_fieldType || MYSQL_TYPE_SHORT == m_fieldType || MYSQL_TYPE_LONG == m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldType)
			return m_lVal;
		return -1;
	}
	Field::operator long long()
	{
		if (MYSQL_TYPE_TINY == m_fieldType || MYSQL_TYPE_SHORT == m_fieldType || MYSQL_TYPE_LONG == m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldType)
			return m_llVal;
		return -1;
	}
	Field::operator float()
	{
		if (MYSQL_TYPE_FLOAT == m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldType)
			return m_fVal;
		return -1;
	}
	Field::operator double()
	{
		if (MYSQL_TYPE_FLOAT == m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldType)
			return m_dVal;
		return -1;
	}
	Field::operator std::string()
	{
		if (MYSQL_TYPE_STRING == m_fieldType || MYSQL_TYPE_VAR_STRING == m_fieldType)
			return m_strVal;
		return "";
	}
	std::ostream& DATABASE::operator<<(std::ostream& os, const Field& field)
	{
		switch (field.m_fieldType)
		{
		case MYSQL_TYPE_TINY:           os << field.m_iVal;         break;
		case MYSQL_TYPE_SHORT:          os << field.m_iVal;         break;
		case MYSQL_TYPE_LONG:           os << field.m_lVal;         break;
		case MYSQL_TYPE_LONGLONG:       os << field.m_llVal;        break;
		case MYSQL_TYPE_FLOAT:          os << field.m_fVal;         break;
		case MYSQL_TYPE_DOUBLE:         os << field.m_dVal;         break;
		case MYSQL_TYPE_STRING:         os << field.m_strVal;       break;
		case MYSQL_TYPE_VAR_STRING:     os << field.m_strVal;       break;
		default:                        os << field.m_iVal;         break;
		}
		return os;
	}
	void Field::Sprintfs(const Field& field, int& len, char(&strsql)[SQL_SIZE])
	{
		switch (field.m_fieldType)
		{
		case MYSQL_TYPE_TINY:           len += sprintf_s(strsql + len, SQL_SIZE - len, "%d", field.m_iVal);             break;
		case MYSQL_TYPE_SHORT:          len += sprintf_s(strsql + len, SQL_SIZE - len, "%d", field.m_iVal);             break;
		case MYSQL_TYPE_LONG:           len += sprintf_s(strsql + len, SQL_SIZE - len, "%ld", field.m_lVal);            break;
		case MYSQL_TYPE_LONGLONG:       len += sprintf_s(strsql + len, SQL_SIZE - len, "%lld", field.m_llVal);          break;
		case MYSQL_TYPE_FLOAT:          len += sprintf_s(strsql + len, SQL_SIZE - len, "%f", field.m_fVal);             break;
		case MYSQL_TYPE_DOUBLE:         len += sprintf_s(strsql + len, SQL_SIZE - len, "%f", field.m_dVal);             break;
		case MYSQL_TYPE_STRING:         len += sprintf_s(strsql + len, SQL_SIZE - len, "'%s'", field.m_strVal.c_str()); break;
		case MYSQL_TYPE_VAR_STRING:     len += sprintf_s(strsql + len, SQL_SIZE - len, "'%s'", field.m_strVal.c_str()); break;
		default:                                                                                                        break;
		}
	}
	void Field::GetField(enum_field_types fieldType, const char* val)
	{
		m_dVal = 0;
		m_strVal.clear();
		m_bDirty = false;
		m_fieldType = fieldType;
		if (val)
		{
			switch (m_fieldType)
			{
			case MYSQL_TYPE_TINY:           m_iVal = atoi(val);                         break;
			case MYSQL_TYPE_SHORT:          m_iVal = atoi(val);                         break;
			case MYSQL_TYPE_LONG:           m_lVal = static_cast<long>(_atoi64(val));   break;
			case MYSQL_TYPE_LONGLONG:       m_llVal = _atoi64(val);                     break;
			case MYSQL_TYPE_FLOAT:          m_dVal = atof(val);                         break;
			case MYSQL_TYPE_DOUBLE:         m_dVal = atof(val);                         break;
			case MYSQL_TYPE_STRING:         m_strVal = val;                             break;
			case MYSQL_TYPE_VAR_STRING:     m_strVal = val;                             break;
			default:                                                                    break;
			}
		}
	}

	template<typename Index, Index size, const char* tableName>
	bool Record<Index, size, tableName>::s_default = false;
	template<typename Index, Index size, const char* tableName>
	Record<Index, size, tableName> Record<Index, size, tableName>::s_defaultRecord;
	template<typename Index, Index size, const char* tableName>
	unsigned int Record<Index, size, tableName>::s_keyIndex = 0;
	template<typename Index, Index size, const char* tableName>
	std::string Record<Index, size, tableName>::s_fieldsName[size];
	template<typename Index, Index size, const char* tableName>
	Record<Index, size, tableName>::Record(Key key)
	{
		if (!Record::s_default)
		{
			char strsql[SQL_SIZE];
			memset(strsql, 0, SQL_SIZE);
			int len = sprintf_s(strsql, SQL_SIZE, "SELECT * FROM %s WHERE %s=0 LIMIT 1;", tableName, g_strKey);
			MysqlDB::GetMysqlHandle()->InitDefaultRecord(strsql, *this);
		}
		for (size_t i = 0; i < size; ++i)
		{
			m_fieldArr[i] = Record::s_defaultRecord.m_fieldArr[i];
		}
		SetKey(key);
	}
	template<typename Index, Index size, const char* tableName>
	Key Record<Index, size, tableName>::GetKey()
	{
		return m_fieldArr[Record::s_keyIndex].m_llVal;
	}
	template<typename Index, Index size, const char* tableName>
	void Record<Index, size, tableName>::SetKey(Key key)
	{
		m_fieldArr[Record::s_keyIndex].m_llVal = key;
	}
	template<typename Index, Index size, const char* tableName>
	int Record<Index, size, tableName>::GetInt(Index index) const
	{
		if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
			return m_fieldArr[index].m_iVal;
		return -1;
	}
	template<typename Index, Index size, const char* tableName>
	void Record<Index, size, tableName>::SetInt(Index index, int val)
	{
		if (index == Record::s_keyIndex) return;
		if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
		{
			m_fieldArr[index].m_iVal = val;
			m_fieldArr[index].m_bDirty = true;
		}
	}
	template<typename Index, Index size, const char* tableName>
	long Record<Index, size, tableName>::GetLong(Index index) const
	{
		if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
			return m_fieldArr[index].m_lVal;
		return -1;
	}
	template<typename Index, Index size, const char* tableName>
	void Record<Index, size, tableName>::SetLong(Index index, long val)
	{
		if (index == Record::s_keyIndex) return;
		if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
		{
			m_fieldArr[index].m_lVal = val;
			m_fieldArr[index].m_bDirty = true;
		}
	}
	template<typename Index, Index size, const char* tableName>
	long long Record<Index, size, tableName>::GetLonglong(Index index) const
	{
		if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
			return m_fieldArr[index].m_llVal;
		return -1;
	}
	template<typename Index, Index size, const char* tableName>
	void Record<Index, size, tableName>::SetLonglong(Index index, long long val)
	{
		if (index == Record::s_keyIndex) return;
		if (MYSQL_TYPE_TINY == m_fieldArr[index].m_fieldType || MYSQL_TYPE_SHORT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONG == m_fieldArr[index].m_fieldType || MYSQL_TYPE_LONGLONG == m_fieldArr[index].m_fieldType)
		{
			m_fieldArr[index].m_llVal = val;
			m_fieldArr[index].m_bDirty = true;
		}
	}
	template<typename Index, Index size, const char* tableName>
	float Record<Index, size, tableName>::GetFloat(Index index) const
	{
		if (MYSQL_TYPE_FLOAT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldArr[index].m_fieldType)
			return m_fieldArr[index].m_fVal;
		return -1;
	}
	template<typename Index, Index size, const char* tableName>
	void Record<Index, size, tableName>::SetFloat(Index index, float val)
	{
		if (index == Record::s_keyIndex) return;
		if (MYSQL_TYPE_FLOAT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldArr[index].m_fieldType)
		{
			m_fieldArr[index].m_fVal = val;
			m_fieldArr[index].m_bDirty = true;
		}
	}
	template<typename Index, Index size, const char* tableName>
	double Record<Index, size, tableName>::GetDouble(Index index) const
	{
		if (MYSQL_TYPE_FLOAT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldArr[index].m_fieldType)
			return m_fieldArr[index].m_dVal;
		return -1;
	}
	template<typename Index, Index size, const char* tableName>
	void Record<Index, size, tableName>::SetDouble(Index index, double val)
	{
		if (index == Record::s_keyIndex) return;
		if (MYSQL_TYPE_FLOAT == m_fieldArr[index].m_fieldType || MYSQL_TYPE_DOUBLE == m_fieldArr[index].m_fieldType)
		{
			m_fieldArr[index].m_dVal = val;
			m_fieldArr[index].m_bDirty = true;
		}
	}
	template<typename Index, Index size, const char* tableName>
	std::string Record<Index, size, tableName>::GetString(Index index) const
	{
		if (MYSQL_TYPE_STRING == m_fieldArr[index].m_fieldType || MYSQL_TYPE_VAR_STRING == m_fieldArr[index].m_fieldType)
			return m_fieldArr[index].m_strVal;
		return -1;
	}
	template<typename Index, Index size, const char* tableName>
	void Record<Index, size, tableName>::SetString(Index index, const std::string& val)
	{
		if (index == Record::s_keyIndex) return;
		if (MYSQL_TYPE_STRING == m_fieldArr[index].m_fieldType || MYSQL_TYPE_VAR_STRING == m_fieldArr[index].m_fieldType)
		{
			m_fieldArr[index].m_strVal = val;
			m_fieldArr[index].m_bDirty = true;
		}
	}
	template<typename Index, Index size, const char* tableName>
	const Field& Record<Index, size, tableName>::operator[](unsigned int index) const
	{
		return m_fieldArr[index];
	}
	template<typename Index, Index size, const char* tableName>
	size_t Record<Index, size, tableName>::Size() const
	{
		return static_cast<size_t>(size);
	}
	template<typename Index, Index size, const char* tableName>
	bool Record<Index, size, tableName>::Insert()
	{
		static char strsql[SQL_SIZE];
		memset(strsql, 0, SQL_SIZE);
		int len = sprintf_s(strsql, SQL_SIZE, "INSERT INTO %s VALUES (", tableName);
		for (size_t i = 0; i < size; ++i)
		{
			Field::Sprintfs(m_fieldArr[i], len, strsql);
			if (i < (size - 1))
			{
				len += sprintf_s(strsql + len, SQL_SIZE - len, ",");
			}
		}
		len += sprintf_s(strsql + len, SQL_SIZE - len, ");");
		return MysqlDB::GetMysqlHandle()->Query(strsql);
	}
	template<typename Index, Index size, const char* tableName>
	bool Record<Index, size, tableName>::Delete()
	{
		static char strsql[SQL_SIZE];
		memset(strsql, 0, SQL_SIZE);
		int len = sprintf_s(strsql, SQL_SIZE, "DELETE FROM %s WHERE %s=", tableName, g_strKey);
		Field::Sprintfs(m_fieldArr[Record::s_keyIndex], len, strsql);
		len += sprintf_s(strsql + len, SQL_SIZE - len, ";");
		return MysqlDB::GetMysqlHandle()->Query(strsql);
	}
	template<typename Index, Index size, const char* tableName>
	bool Record<Index, size, tableName>::Update()
	{
		static char strsql[SQL_SIZE];
		memset(strsql, 0, SQL_SIZE);
		int len = sprintf_s(strsql, SQL_SIZE, "UPDATE %s SET ", tableName);
		bool bDirty = false;
		int num = 0;
		for (size_t i = 0; i < size; ++i)
		{
			if (i == Record::s_keyIndex) continue;
			if (m_fieldArr[i].m_bDirty == true)
			{
				if (num) len += sprintf_s(strsql + len, SQL_SIZE - len, ",");
				bDirty = true;
				len += sprintf_s(strsql + len, SQL_SIZE - len, "%s=", Record::s_fieldsName[i].c_str());
				Field::Sprintfs(m_fieldArr[i], len, strsql);
				++num;
			}
		}
		if (!num) return true;
		len += sprintf_s(strsql + len, SQL_SIZE - len, " WHERE %s=", g_strKey);
		Field::Sprintfs(m_fieldArr[Record::s_keyIndex], len, strsql);
		len += sprintf_s(strsql + len, SQL_SIZE - len, ";");
		if (MysqlDB::GetMysqlHandle()->Query(strsql))
		{
			for (size_t i = 0; i < size; ++i)
			{
				if (i == Record::s_keyIndex) continue;
				if (m_fieldArr[i].m_bDirty == true)
				{
					m_fieldArr[i].m_bDirty = false;
				}
			}
			return true;
		}
		return false;
	}
	template<typename Index, Index size, const char* tableName>
	void Record<Index, size, tableName>::GetFields(const MYSQL_FIELD* mysqlField, const MYSQL_ROW& mysqlRow, unsigned int fieldsNum)
	{
		if (!mysqlRow || !mysqlField)
			return;
		assert(fieldsNum == static_cast<unsigned int>(size));
		for (unsigned int i = 0; i < fieldsNum; ++i)
		{
			m_fieldArr[i].GetField(mysqlField[i].type, mysqlRow[i]);
		}
	}
	template<typename Index, Index size, const char* tableName>
	void Record<Index, size, tableName>::InitDefault(const MYSQL_FIELD* mysqlField, unsigned int fieldsNum)
	{
		if (Record::s_default) return;
		assert(fieldsNum == size);
		for (unsigned int i = 0; i < fieldsNum; ++i)
		{
			if (mysqlField[i].flags&PRI_KEY_FLAG || mysqlField[i].flags&AUTO_INCREMENT_FLAG)
			{
				Record::s_keyIndex = i;
			}
			Record::s_fieldsName[i] = mysqlField[i].name;
			Record::s_defaultRecord.m_fieldArr[i].GetField(mysqlField[i].type, mysqlField[i].def);
		}
		Record::s_default = true;
	}

	template<typename RecordType>
	size_t RecordTable<RecordType>::Size() const
	{
		return m_recordTable.size();
	}
	template<typename RecordType>
	bool RecordTable<RecordType>::DeleteRecord(Key key)
	{
		typename std::unordered_map<Key, std::shared_ptr<RecordType>>::iterator ite = m_recordTable.find(key);
		if (ite == m_recordTable.end()) return true;
		if (ite->second->Delete())
		{
			m_recordTable.erase(ite);
			return true;
		}
		return false;
	}
	template<typename RecordType>
	bool RecordTable<RecordType>::InsertRecord(std::shared_ptr<RecordType> record)
	{
		if (m_recordTable.find(record->GetKey()) != m_recordTable.end()) return true;
		if (record->Insert())
		{
			m_recordTable.insert(std::make_pair(record->GetKey(), record));
			return true;
		}
		return false;
	}
	template<typename RecordType>
	void RecordTable<RecordType>::CreateTable(MYSQL_RES* mysqlRes, const MYSQL_FIELD* mysqlField, MYSQL_ROW& mysqlRow, unsigned int fieldsNum, unsigned __int64 rowsNum)
	{
		if (!mysqlField)
			return;
		m_recordTable.clear();
		RecordType::InitDefault(mysqlField, fieldsNum);
		for (unsigned __int64 row = 0; row < rowsNum; ++row)
		{
			mysqlRow = mysql_fetch_row(mysqlRes);
			std::shared_ptr<RecordType> record(RecordType::CreateNew());
			record->GetFields(mysqlField, mysqlRow, fieldsNum);
			m_recordTable.insert(std::make_pair(record->GetKey(), record));
		}
	}
	template<typename RecordType>
	typename std::unordered_map<Key, std::shared_ptr<RecordType>>::iterator RecordTable<RecordType>::find(Key key)
	{
		return m_recordTable.find(key);
	}
	template<typename RecordType>
	typename std::unordered_map<Key, std::shared_ptr<RecordType>>::iterator RecordTable<RecordType>::begin()
	{
		return m_recordTable.begin();
	}
	template<typename RecordType>
	typename std::unordered_map<Key, std::shared_ptr<RecordType>>::iterator RecordTable<RecordType>::end()
	{
		return m_recordTable.end();
	}

	bool MysqlDB::Query(const char(&strsql)[SQL_SIZE]) const
	{
		if (!m_mysql) exit(-1);
		if (mysql_query(m_mysql, strsql))
		{
			std::cout << strsql << " " << mysql_error(m_mysql) << std::endl;
			return false;
		}
		return true;
	}
	template<typename RecordType>
	void MysqlDB::InitDefaultRecord(const char(&strsql)[SQL_SIZE], const RecordType& recordType)
	{
		if (mysql_query(m_mysql, strsql))
		{
			std::cout << strsql << " " << mysql_error(m_mysql) << std::endl;
			exit(-1);
		}
		m_mysqlRes = mysql_store_result(m_mysql);
		if (!m_mysqlRes) exit(-1);
		unsigned int fieldsNum = mysql_num_fields(m_mysqlRes);
		m_mysqlField = mysql_fetch_field(m_mysqlRes);
		RecordType::InitDefault(m_mysqlField, fieldsNum);
		mysql_free_result(m_mysqlRes);
	}
	template<typename RecordType>
	bool MysqlDB::Select(RecordTable<RecordType>& recordTable, const char* tableName, const char* order)
	{
		if (!tableName) return false;
		if (!m_mysql) exit(-1);
		char strsql[SQL_SIZE];
		memset(strsql, 0, SQL_SIZE);
		int len = sprintf_s(strsql, SQL_SIZE, "SELECT * FROM %s", tableName);
		if (order) len += sprintf_s(strsql + len, SQL_SIZE - len, " ORDER BY %s ", order);
		len += sprintf_s(strsql + len, SQL_SIZE - len, ";");
		if (mysql_query(m_mysql, strsql))
		{
			std::cout << strsql << " " << mysql_error(m_mysql) << std::endl;
			return false;
		}
		m_mysqlRes = mysql_store_result(m_mysql);
		if (!m_mysqlRes) return false;
		unsigned int fieldsNum = mysql_num_fields(m_mysqlRes);
		unsigned __int64 rowsNum = mysql_num_rows(m_mysqlRes);
		m_mysqlField = mysql_fetch_field(m_mysqlRes);
		recordTable.CreateTable(m_mysqlRes, m_mysqlField, m_mysqlRow, fieldsNum, rowsNum);
		mysql_free_result(m_mysqlRes);
		return true;
	}
	bool MysqlDB::Connect(const char* host, const char* user, const char* passwd, const char* dbname, unsigned int port, const char* unixSocket, unsigned long clientFlag)
	{
		if (m_mysql) return true;
		m_mysql = mysql_init(m_mysql);
		if (mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, g_encoding)) exit(-1);
		char arg = 1;
		if (mysql_options(m_mysql, MYSQL_OPT_RECONNECT, &arg)) exit(-1);
		if (!mysql_real_connect(m_mysql, host, user, passwd, dbname, port, unixSocket, clientFlag) || mysql_query(m_mysql, "set wait_timeout=86400") || mysql_query(m_mysql, "set interactive_timeout=86400"))
		{
			std::cout << mysql_error(m_mysql) << std::endl;
			mysql_close(m_mysql);
			m_mysql = NULL;
			exit(-1);
		}
		return true;
	}
}

#define DBHandle (DATABASE::MysqlDB::GetMysqlHandle())
template<typename Index, Index size, const char* tableName>
using DBRecord = DATABASE::Record<Index, size, tableName>;
template<typename RecordType>
using DBTble = DATABASE::RecordTable<RecordType>;


#endif /* MYSQLDB_H */
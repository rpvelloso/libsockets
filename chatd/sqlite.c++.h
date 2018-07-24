#ifndef SQLITE_CPP_H_
#define SQLITE_CPP_H_

#include <exception>
#include <memory>
#include <vector>
#include <type_traits>
#include <iostream>

#include "sqlite3.h"

inline void closeDB(sqlite3 *h) {
	if (h != nullptr)
		sqlite3_close(h);
}

class SQLiteQuery;
class SQLiteTransactionGuard;

class SQLiteDB {
friend class SQLiteQuery;
public:
	SQLiteDB(const std::string &filename) :
		filename(filename),
		dbHandle(nullptr, closeDB) {

		sqlite3 *handle;

		auto res = sqlite3_open(this->filename.c_str(), &handle);
		if (res != SQLITE_OK)
			throw std::runtime_error(sqlite3_errstr(res));
		dbHandle.reset(handle);
	};

	template<class ... Types>
	SQLiteQuery createQuery(const std::string &queryString, const Types& ... values);
	SQLiteQuery createQuery(const std::string &queryString);
	SQLiteTransactionGuard startTransaction();

	sqlite3_int64 lastInsertRowID() {
		return sqlite3_last_insert_rowid(dbHandle.get());
	};

	void setVerbose(bool v) {
		verbose = v;
	};
	bool getVerbose() {
		return verbose;
	};
private:
	bool verbose = false;
	std::string filename;
	std::unique_ptr<sqlite3, decltype(&closeDB)> dbHandle;
};

inline int bindValue(sqlite3_stmt *stmt, int c, const int &value) {
	return sqlite3_bind_int(stmt, c, value);
}

inline int bindValue(sqlite3_stmt *stmt, int c, const sqlite_int64 &value) {
	return sqlite3_bind_int64(stmt, c, value);
}

inline int bindValue(sqlite3_stmt *stmt, int c, const double &value) {
	return sqlite3_bind_double(stmt, c, value);
}

inline int bindValue(sqlite3_stmt *stmt, int pos, const std::string &value) {
	return sqlite3_bind_text(stmt, pos, value.c_str(), -1, SQLITE_TRANSIENT);
}

inline int bindValue(sqlite3_stmt *stmt, int pos, char * const &value) {
	return sqlite3_bind_text(stmt, pos, value, -1, SQLITE_TRANSIENT);
}

inline int bindValue(sqlite3_stmt *stmt, int pos, const std::vector<char> &value) {
	return sqlite3_bind_blob(stmt, pos, (const void *)&value[0], value.size(), SQLITE_TRANSIENT);
}

template<class T>
void _bindValues(sqlite3_stmt *stmt, int k, const T& first) {
	auto res = bindValue(stmt, k, first);
	if (res != SQLITE_OK)
		throw std::runtime_error(sqlite3_errstr(res));
}

template<class T, class ...Types>
void _bindValues(sqlite3_stmt *stmt, int k, const T& first, const Types&...args) {
	auto res = bindValue(stmt, k, first);
	if (res != SQLITE_OK)
		throw std::runtime_error(sqlite3_errstr(res));
	_bindValues(stmt, k+1, args...);
}

inline void finalizeStmt(sqlite3_stmt *stmt) {
	if (stmt != nullptr)
		sqlite3_finalize(stmt);
}

class SQLiteResult;

class SQLiteQuery {
	friend SQLiteResult;
public:
	SQLiteQuery(SQLiteDB &db) :
		db(db),
		stmt(nullptr, finalizeStmt) {
	};
	
	SQLiteQuery(SQLiteDB &db, const std::string &query) : 
		db(db),
		stmt(nullptr, finalizeStmt),
		query(query) {
			
		prepare();
	};
	
	SQLiteQuery &operator=(const std::string &query) {
		this->query = query;
		prepare();
		return *this;
	}
	
	template<class ... Types>
	void bindValues(const Types&... args) {
		_bindValues(stmt.get(), 1, args...);
	}

	template<class T>
	void bind(int pos, const T& value) {
		auto res = bindValue(stmt.get(), pos, value);
		if (res != SQLITE_OK)
			throw std::runtime_error(sqlite3_errstr(res));
	}
	
	void bind(int pos, const char *value) {
		std::string str(value);
		auto res = bindValue(stmt.get(), pos, str);
		if (res != SQLITE_OK)
			throw std::runtime_error(sqlite3_errstr(res));
	}

	void execute() {
		if (db.getVerbose())
			std::cerr << sqlite3_expanded_sql(stmt.get()) << std::endl << std::endl;

		auto res = sqlite3_step(stmt.get());
		if (res != SQLITE_OK && res != SQLITE_DONE)
			throw std::runtime_error(sqlite3_errstr(res));
	};
	
	void reset() {
		auto res = sqlite3_reset(stmt.get());
		if (res != SQLITE_OK)
			throw std::runtime_error(sqlite3_errstr(res));
	};

	SQLiteResult getResult();

private:
	SQLiteDB &db;
	std::shared_ptr<sqlite3_stmt> stmt;
	std::string query;
	
	void prepare() {
		sqlite3_stmt *stmtHandle;
		
		auto res = sqlite3_prepare(
			db.dbHandle.get(),
			query.c_str(),
			query.size(),
			&stmtHandle,
			nullptr);
		
		if (res != SQLITE_OK)
			throw std::runtime_error(sqlite3_errstr(res));
				
		stmt.reset(stmtHandle, finalizeStmt);
	}
};

template<class ... Types>
SQLiteQuery SQLiteDB::createQuery(const std::string &queryString, const Types& ... values) {
	SQLiteQuery query(*this, queryString);
	query.bindValues(values...);
	return query;
}

inline SQLiteQuery SQLiteDB::createQuery(const std::string &queryString) {
	return SQLiteQuery(*this, queryString);
}

class SQLiteTransactionGuard {
public:
	SQLiteTransactionGuard(SQLiteDB &db) : db(db) {
		db.createQuery("BEGIN TRANSACTION;").execute();
	};

	SQLiteTransactionGuard(SQLiteTransactionGuard &&rhs) : finished(rhs.finished), db(rhs.db) {
		rhs.finished = true; // to prevent rollback when destroying moved object
	};

	void commit() { // must be called explicitly, otherwise rolls back when destroying transaction obj
		db.createQuery("COMMIT;").execute();
		finished = true;
	};

	void rollback() {
		db.createQuery("ROLLBACK;").execute();
		finished = true;
	};

	~SQLiteTransactionGuard() {
		if (!finished)
			rollback();
	}
private:
	bool finished = false;
	SQLiteDB &db;
	SQLiteTransactionGuard(SQLiteTransactionGuard &) = delete;
};

inline SQLiteTransactionGuard SQLiteDB::startTransaction() {
	return SQLiteTransactionGuard(*this);
};

template<class T>
T getColumn(sqlite3_stmt *stmt, int c) {
	T result;
	throw std::runtime_error("invalid or unhandled data type - column");
	return result;
};

template<>
inline int getColumn<int>(sqlite3_stmt *stmt, int c) {
	return sqlite3_column_int(stmt, c);
}

template<>
inline sqlite_int64 getColumn<sqlite_int64>(sqlite3_stmt *stmt, int c) {
	return sqlite3_column_int64(stmt, c);
}

template<>
inline double getColumn<double>(sqlite3_stmt *stmt, int c) {
	return sqlite3_column_double(stmt, c);
}

template<>
inline std::string getColumn<std::string>(sqlite3_stmt *stmt, int c) {
	auto pStr = (const char *)sqlite3_column_text(stmt, c);
	auto len = sqlite3_column_bytes(stmt, c);
	if (pStr == nullptr || len <= 0)
		return std::string();
	return std::string((const char *)pStr, len);
}

template<>
inline std::vector<char> getColumn<std::vector<char>>(sqlite3_stmt *stmt, int c) {
	auto pBlob = (const char *)sqlite3_column_blob(stmt, c);
	auto len = sqlite3_column_bytes(stmt, c);
	if (pBlob == nullptr || len <= 0)
		return std::vector<char>();
	std::vector<char> result(len);
	std::copy(pBlob, pBlob + len, &result[0]);
	return result;
}

template<class T>
void getRecord(sqlite3_stmt *stmt, int k, T &first) {
	first = getColumn<T>(stmt, k);
}

template<class T, class ...Types>
void getRecord(sqlite3_stmt *stmt, int k, T &first, Types&...args) {
	first = getColumn<T>(stmt, k);
	getRecord(stmt, k+1, args...);
}

class SQLiteResult {
public:
	SQLiteResult(SQLiteQuery &query) : stmt(query.stmt) {};
	
	template<class ... Types>
	void fetch(Types&...args) {
		getRecord(stmt.get(), 0, args...);
	}
	
	bool next() {
		auto res = sqlite3_step(stmt.get());
		if (res == SQLITE_ROW)
			return true;
		if (res == SQLITE_DONE)
			return false;
		
		throw std::runtime_error(sqlite3_errstr(res));
	};
private:
	std::shared_ptr<sqlite3_stmt> stmt;
};

inline SQLiteResult SQLiteQuery::getResult() {
	if (db.getVerbose())
		std::cerr << sqlite3_expanded_sql(stmt.get()) << std::endl << std::endl;

	return SQLiteResult(*this);
};

#endif // SQLITE_CPP_H_

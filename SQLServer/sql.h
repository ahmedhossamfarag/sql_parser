#pragma once
#include <string>
#include <vector>
#include "state_graph.h"
#include "database.h"
#include "cmp_char.h"
class SQLStatement : public StateParameter
{
public:
	Storage* storage;

	virtual void check_valid() = 0;
	virtual void excute() = 0;
};

class SelectDatabaseStatement : public SQLStatement
{
public:
	std::string database_name;

	void check_valid() override {};
	void excute() override;
};

class CreateDatabaseStatement : public SQLStatement
{
public:
	std::string database_name;

	void check_valid() override {};
	void excute() override;
};

class OnTableStatement : public SQLStatement
{
public:
	std::string table_name; 
	void check_valid() override;
};


class CreateTableStatement : public OnTableStatement
{
public:
	std::vector<TableField> fields;

	void excute() override;
};

class DropDatabaseStatement : public SQLStatement
{
public:
	std::string database_name;

	void check_valid() override {};
	void excute() override;
};

class DropTableStatement : public OnTableStatement
{
public:

	void excute() override;
};

class InsertStatement : public OnTableStatement
{
public:
	std::vector<std::string> fields;
	std::vector <std::string> values;

	void excute();
};



class ConditionalStatement : public OnTableStatement
{
public:
	WhereClause where;
};

class SelectStatement : public ConditionalStatement
{
public:
	std::vector<std::string> fields;

	void excute() override;
};

class DeleteStatement : public ConditionalStatement
{
public:

	void excute() override;
};

class UpdateStatement : public ConditionalStatement
{
public:
	std::vector<std::string> fields;
	std::vector <std::string> values;

	void excute();
};
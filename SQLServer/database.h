#pragma once
#include <vector>
#include <string>
#include "bptree.h"
#define DBstart 2

enum FieldType : int
{
	Int,
	Float,
	Boolean,
	DateTime,
	String,
};

enum FieldConstrains
{
	PrimaryKey,
	AutoIncrement,
	NotNull
};

class TableField {
public:
	std::string field_name;
	FieldType type = String;
	uint8_t field_size = sizeof(char);
	std::string defaultValue;
	std::vector<FieldConstrains> constrains;
};

class Field
{
public:

	TableField tfield;

	Field(TableField tf = TableField{});
	bool is_valid_default();
	bool is_key();
	bool check_value(std::string value);
	void encode(std::string value, char* c);
	std::string decode(char* value);
	int size();
	void write_to(char* data);
	void read_from(char* data);
};

class Table
{
public:
	std::string table_name;
	std::vector<Field> fields;
	BpTree tree;

	Table(std::string name);
	int size();
	int record_size();
	int key_size();
	void write_to(char* data);
	void read_from(char* data);
	void validate();

};

class Record
{
public:
	std::vector<Field>* fields;
	std::string* values;

	Record(std::vector<Field>* fields);
	int size();
	char* encode();
	void decode(char* data);
};

class Database
{
public: 
	std::string database_name;
	std::vector<Table> tables;
	FileController* file_cntr;
	BlockController* blk_cntr;

	Database(std::string name);
	int size();
	void write_to(char* data);
	void read_from(char* data);
};

class Storage
{
public:
	std::vector<Database> databases;
	Database* selected_database = nullptr;
};



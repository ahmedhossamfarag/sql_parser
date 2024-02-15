#include "database.h"
#include "str.h"
#include "vec.h"
#include <algorithm>
#include <iostream>
#define DATESIZE 10

Field::Field(TableField tf)
{
	this->tfield = tf;
	switch (tf.type)
	{
	case Int: {
		tfield.field_size = sizeof(int);
		break;
	}
	case Float: {
		tfield.field_size = sizeof(float);
		break;
	}
	case Boolean: {
		tfield.field_size = sizeof(bool);
		break;
	}
	case DateTime: {
		tfield.field_size = DATESIZE;
		break;
	}
	}
}

bool Field::check_value(std::string df)
{
	switch (tfield.type)
	{
	case Int: return str_is_int(df);
	case Float: return str_is_float(df);
	case Boolean: return str_is_bool(df);
	case DateTime: return str_is_date(df);
	default:
		return tfield.field_size >= tfield.defaultValue.size();
	}
}

bool Field::is_valid_default()
{
	auto df = tfield.defaultValue;
	if (!df.empty()) {
		return check_value(df);
	}
	return true;
}

int Field::size()
{
	return 1 + tfield.field_name.length() + 1 + 1 + 1 + tfield.defaultValue.length() + 1 + tfield.constrains.size();
}

void Field::encode(std::string value, char* c)
{
	switch (tfield.type)
	{
	case Int: {
		int* i = new int(str_to_int(value));
		char* r = (char*)i;
		std::copy(r, r + tfield.field_size, c);
		break;
	}
	case Float: {
		double* i = new double(str_to_float(value));
		char* r = (char*)i;
		std::copy(r, r + tfield.field_size, c);
		break;
	}
	case Boolean: {
		*c = str_to_bool(value);
		break;
	}
	default: {
		std::fill(c, c + tfield.field_size, 0);
		std::copy_n(value.begin(), (int)fmin(value.size(), tfield.field_size), c);
		break;
	}
	}
}

bool Field::is_key()
{
	return std::any_of(tfield.constrains.begin(), tfield.constrains.end(), [](FieldConstrains c) {return c == PrimaryKey; });
}

std::string Field::decode(char* c)
{
	switch (tfield.type)
	{
	case Int: {
		int* i = new int();
		char* r = (char*)i;
		std::copy(c, c + tfield.field_size, r);
		return int_to_string(*i);
	}
	case Float: {
		double* i = new double();
		char* r = (char*)i;
		std::copy(c, c + tfield.field_size, r);
		return float_to_string(*i);
	}
	case Boolean: {
		return bool_to_string(*c);
	}
	default: {
		auto s = new char[tfield.field_size];
		std::copy(c, c + tfield.field_size, s);
		return s;
	}
	}
}

void Field::write_to(char* data)
{
	*data = tfield.field_name.size();
	data++;
	std::copy(tfield.field_name.begin(), tfield.field_name.end(), data);
	data += tfield.field_name.length();
	*data = tfield.type;
	data++;
	*data = tfield.field_size;
	data++;
	*data = tfield.defaultValue.size();
	data++;
	std::copy(tfield.defaultValue.begin(), tfield.defaultValue.end(), data);
	data += tfield.defaultValue.length();
	*data = tfield.constrains.size();
	data++;
	std::copy(tfield.constrains.begin(), tfield.constrains.end(), data);
	data += tfield.constrains.size();
}

void Field::read_from(char* data)
{
	int size = *data;
	tfield.field_name.resize(*data);
	data++;
	std::copy(data, data + size, tfield.field_name.begin());
	data += size;
	tfield.type = (FieldType)*data;
	data ++;
	tfield.field_size = *data;
	data++;
	size = *data;
	tfield.defaultValue.resize(*data);
	data++;
	std::copy(data, data + size, tfield.defaultValue.begin());
	data += size;
	size = *data;
	data++;
	for (auto i = data; i < data + size; i++)
	{
		tfield.constrains.push_back((FieldConstrains)*i);
	}
	data += size;
}

int Table::size()
{
	return 1 + table_name.length() + 1 + vector_sum_int<Field>(fields, [](Field f) {return f.size(); }) + 1;
}

Table::Table(std::string name) : tree(BpTree())
{
	this->table_name = name;
}

void Table::write_to(char* data)
{
	*data = table_name.length();
	data++;
	std::copy(table_name.begin(), table_name.end(), data);
	data += table_name.length();
	*data = fields.size();
	data++;
	for (auto i = fields.begin(); i < fields.end(); i++)
	{
		(*i).write_to(data);
		data += (*i).size();
	}

	*data = tree.root;
	data++;
}

void Table::validate()
{
	for (auto i = fields.begin(); i < fields.end(); i++)
	{
		if (std::any_of(i + 1, fields.end(), [=](Field f) {return f.tfield.field_name == (*i).tfield.field_name; }))
			throw std::exception("Duplicate Field Name");
	}
}

int Table::record_size()
{
	return vector_sum_int<Field>(fields, [](Field f) {return f.tfield.field_size; });
}

int Table::key_size()
{
	return vector_sum_int<Field>(fields, [](Field f) {return f.is_key() ? f.tfield.field_size : 0; });
}

void Table::read_from(char* data)
{
	int size = *data;
	table_name.resize(size);
	data++;
	std::copy(data, data + size, table_name.begin());
	data += size;
	size = *data;
	data++;
	for (auto i = 0; i < size; i++)
	{
		Field f;
		f.read_from(data);
		fields.push_back(f);
		data += f.size();
	}
	tree.root = *data;
	data++;
}

Database::Database(std::string name)
{
	this->database_name = name;
	file_cntr = new FileController(name.c_str());
	blk_cntr = new BlockController(file_cntr);
}

int Database::size()
{
	int s = 1 + database_name.length() + 1;
	for (auto i = tables.begin(); i < tables.end(); i++)
	{
		s += (*i).size();
	}
	return s;
}

void Database::write_to(char* data)
{
	char* st = data;
	*data = database_name.length();
	data++;
	std::copy(database_name.begin(), database_name.end(), data);
	data += database_name.length();
	*data = tables.size();
	data++;
	for (auto i = tables.begin(); i < tables.end(); i++)
	{
		i->write_to(data);
		data += i->size();
		i->tree.root_ps = data - st - 1 + DBstart;
	}
}

void Database::read_from(char* data)
{
	char* st = data;
	int size = *data;
	database_name.resize(size);
	data++;
	std::copy(data, data + size, database_name.begin());
	data += size;
	size = *data;
	data++;
	for (auto i = 0; i < size; i++)
	{
		Table t{""};
		t.read_from(data);
		t.tree.param = NodeParamter{ file_cntr, blk_cntr, t.key_size(), t.record_size() };
		tables.push_back(t);
		data += t.size();
		t.tree.root_ps = data - st - 1 + DBstart;
	}
}

Record::Record(std::vector<Field>* fields)
{
	this->fields = fields;
	this->values = new std::string[fields->size()];
}

int Record::size()
{
	return vector_sum_int<Field>(*fields, [](Field f) {return f.tfield.field_size; });
}

char* Record::encode()
{
	char* data = new char[size()];
	char* d = data;
	for (size_t i = 0; i < fields->size(); i++)
	{
		fields->at(i).encode(values[i], data);
		data += fields->at(i).tfield.field_size;
	}
	return d;
}

void Record::decode(char* data)
{
	for (size_t i = 0; i < fields->size(); i++)
	{
		values[i] = (fields->at(i).decode(data));
		data += fields->at(i).tfield.field_size;
	}
}

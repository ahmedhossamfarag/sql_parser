#pragma once
#include "sql.h"
#include "state_graph.h"

class SelectDatabase : public State
{

	State s1, s2{ true };
public:
	StateParameter* create_object(StateParameter* p) override;
	void on_init(StateParameter* p) override;
};

class CreateDatabese : public State
{
	State s1, s2{ true };
public:
	StateParameter* create_object(StateParameter* p) override;
	void on_init(StateParameter* p) override;
};

class CreateTable : public State 
{
	State s1, s2, s3, s4, s5, s6, s7{ true }, s8, s9, s10, s11, s12, s13;
	std::string* types = new std::string[4]{ "int", "float", "boolean", "datetime", };
	std::string* constrains = new std::string[4]{ "primarykey", "autoincrement", "notnull" };
public:
	StateParameter* create_object(StateParameter* p) override;
	void on_init(StateParameter* p) override;
};

class DropDatabese : public State
{
	State s1, s2{ true };
public:
	StateParameter* create_object(StateParameter* p) override;
	void on_init(StateParameter* p) override;
};

class DropTable : public State 
{
	State s1, s2{ true };
public:
	StateParameter* create_object(StateParameter* p) override;
	void on_init(StateParameter* p) override;
};

class Insert : public State
{
	State s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12{ true };

public:
	StateParameter* create_object(StateParameter* p) override;
	void on_init(StateParameter* p) override;
};

class Where : public State
{
	std::string* m = new std::string[6]{ ">=", "<=", "!=", "=", ">", "<" };
	std::string* l = new std::string[2]{ "&", "|" };
	State s1, s2, s3, s4, s5{ true };

public:
	void on_init(StateParameter* p) override;
};

class Select : public State
{
	State s1, s2, s3, s4, s5, s6{ true };
	Where w;
public:
	StateParameter* create_object(StateParameter* p) override;
	void on_init(StateParameter* p) override;
};

class Delete : public State
{
	State s1, s2, s3, s4{ true };
	Where w;
public:
	StateParameter* create_object(StateParameter* p) override;
	void on_init(StateParameter* p) override;
};

class Update : public State
{
	State s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11{ true };
	Where w;
public:
	StateParameter* create_object(StateParameter* p) override;
	void on_init(StateParameter* p) override;
};

class SQL : public State
{

	SelectDatabase db;
	CreateDatabese cdb;
	CreateTable ct;
	DropDatabese ddb;
	DropTable dt;
	Insert in;
	Select se;
	Delete de;
	Update up;

	State s1, s2, s3, s4;
public:
	void on_init(StateParameter* p) override;
};

State* build_sql_graph(std::string statement);
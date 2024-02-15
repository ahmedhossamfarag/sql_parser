#pragma once
#include "database.h"
#include "str.h"
#include "vec.h"
#include <map>
#include "cmp_char.h"

std::vector<Database>::iterator get_db(Storage* storage, std::string name);

bool file_exist(std::string name);

bool db_exist(Storage* storage, std::string name);

void create_fl(std::string name);

void write_db(Database* db);

void read_db(Database* db);

bool tb_exist(Database* db, std::string name);

std::vector<Field> create_fields(std::vector<TableField>* tf);

void format_fields(std::vector<Field>* fl);

void create_tr(Database* db, Table* tb);

std::vector<Table>::iterator get_tb(Database* db, std::string name);

std::map<std::vector<Field>::iterator, std::string> map_fl_vl(std::vector<Field>* fl, std::vector<std::string>* flds, std::vector<std::string>* vls);

char* create_rc(Table* tb, std::vector<std::string>* flds, std::vector<std::string>* vls);

void display(Table* tb, char* c);

bool* create_bool_map(Table* tb, std::vector<std::string>* flds);

std::vector<Field>::iterator get_fld(Table* tb, std::string name);

int get_fld_ps(Table* tb, std::vector<Field>::iterator fld);

CharCondition create_cnd(Table* t, WhereClause wh);
#include "sql.h"
#include "database.h"
#include <algorithm>
#include "sql_parse.h"

void SelectDatabaseStatement::excute()
{
	/*auto db = std::find_if(storage->databases.begin(), storage->databases.end(), [=](Database d) {return d.database_name == database_name; });
	if (db == storage->databases.end())
		throw std::exception("Database Not Found");
	storage->selected_database = &(*db);*/

	//get db
	auto db = get_db(storage, database_name);
	if (db == storage->databases.end())
	{
		//check file
			//if(!file_exist(database_name))
			throw std::exception("Database douse not exist: ");
		//create db obj
			Database d{ database_name };
		//read file
			read_db(&d);
		//select
			storage->databases.push_back(d);
			storage->selected_database = (storage->databases.end() - 1)._Ptr;
	}
	else 
	{
		storage->selected_database = db._Ptr;
	}
}

void CreateDatabaseStatement::excute()
{
	/*this->check_valid();
	storage->databases.push_back(Database(database_name));*/
	
	//check name
	/*if (db_exist(storage, database_name) || file_exist(database_name))
		throw std::exception("Database already exists");*/
	if (db_exist(storage, database_name))
		throw std::exception("Database already exists");
	//create file
		//create_fl(database_name);
	//create db & add
	Database db{ database_name };
	storage->databases.push_back(db);
	//write db
		write_db(&db);
}

void OnTableStatement::check_valid()
{
	if (!storage->selected_database)
		throw std::exception("No Database Selected");
}

void CreateTableStatement::excute()
{
	/*Table* t = new Table{ table_name };
	for (auto i = fields.begin(); i < fields.end(); i++)
	{
		t->fields.push_back(Field(*i));
		if (!t->fields.back().is_valid_default())
			throw std::exception(("Invalid Default Value : " + (*i).field_name).c_str());
	}
	t->validate();
	storage->selected_database->tables.push_back(*t);
	NodeParamter np{ storage->selected_database->file_cntr, storage->selected_database->blk_cntr, t->key_size(), t->record_size() };
	t->tree.param = np;
	auto st = create_rec_node(np, 0);
	st.save();
	t->tree.root = st.blck_indx;*/

	//check name
	if(tb_exist(storage->selected_database, table_name))
		throw std::exception("Table already exists");
	//create fields
	auto flds = create_fields(&fields);
	//check fields
	for (auto pt = flds.begin(); pt < flds.end(); pt++)
	{
		if (!pt->is_valid_default())
			throw std::exception(("Invalid Default Value For : " + pt->tfield.field_name).c_str());
	}
	//format fields
	format_fields(&flds);
	//create tb
	Table tb{ table_name };
	tb.fields = flds;
	// create tree
	create_tr(storage->selected_database, &tb);
	//add table
	storage->selected_database->tables.push_back(tb);
	//write db
	write_db(storage->selected_database);
}

void DropDatabaseStatement::excute()
{
	/*auto dbs = storage->databases;
	auto d = std::find_if(dbs.begin(), dbs.end(), [=](Database db) {return db.database_name == database_name; });
	dbs.erase(d);*/

	//get db
	auto db = get_db(storage, database_name);
	if (db == storage->databases.end())
	{
		//check file
			//if(!file_exist(database_name))
		throw std::exception("Database does not exist");
	}
	else
	{
		//del db
		storage->databases.erase(db);
	}
	//del file
	remove(database_name.c_str());
	storage->selected_database = nullptr;
}

void DropTableStatement::excute()
{
	/*auto db = storage->selected_database->tables;
	auto t = std::find_if(db.begin(), db.end(), [=](Table tb) {return tb.table_name == table_name; });
	db.erase(t);*/

	//get tb
	auto tb = get_tb(storage->selected_database, table_name);
	if (tb == storage->selected_database->tables.end())
	{
		throw std::exception("Table does not exist");
	}
	//del tb
	tb->tree.remove_all();
	storage->selected_database->blk_cntr->erase_block(tb->tree.root);
	storage->selected_database->tables.erase(tb);
	//write db
	write_db(storage->selected_database);
}

void InsertStatement::excute()
{
	//// select table 
	//auto tb = select_tbl(storage, table_name);
	//// map value to char*
	//int rs = tb->record_size();
	//char* rec = new char[rs];
	//auto fp = fields.begin();
	//auto vp = values.begin();
	//for (size_t i = 0; i < fields.size(); i++)
	//{
	//	// get field
	//	auto fl = tb->get_fld(*fp);
	//	// get field index
	//	int n = fl->index;
	//	// encode value
	//	fl->encode(*vp, rec + n * rs);
	//	fp++;
	//	vp++;
	//}
	//// insert	
	//tb->tree.insert(rec);

	//get tb
	auto tb = get_tb(storage->selected_database, table_name);
	if (tb == storage->selected_database->tables.end())
	{
		throw std::exception("Table does not exist");
	}
	//create rc
	char* rc = create_rc(tb._Ptr, &fields, &values);
	if (!fields.empty())
	{
		throw std::exception(("Field does not exist: " + fields[0]).c_str());
	}
	//insert
	tb->tree.insert(rc);
}

void SelectStatement::excute()
{
	//get tb
	auto tb = get_tb(storage->selected_database, table_name);
	if (tb == storage->selected_database->tables.end())
	{
		throw std::exception("Table does not exist");
	}
	//create bool map
	bool* rc = create_bool_map(tb._Ptr, &fields);
	if (!fields.empty())
	{
		throw std::exception(("Field does not exist: " + fields[0]).c_str());
	}
	//create cnd
	auto cnd = create_cnd(tb._Ptr, where);
	//select
	auto it = tb->tree.select(cnd);
	auto crc = it.next();
	while (crc != nullptr)
	{
		display(tb._Ptr, crc);
		crc = it.next();
	}
}

void DeleteStatement::excute()
{
	//get tb
	auto tb = get_tb(storage->selected_database, table_name);
	if (tb == storage->selected_database->tables.end())
	{
		throw std::exception("Table does not exist");
	}
	//create cnd
	auto cnd = create_cnd(tb._Ptr, where);
	//delete
	tb->tree.remove(cnd);
}

void UpdateStatement::excute()
{
	//get tb
	auto tb = get_tb(storage->selected_database, table_name);
	if (tb == storage->selected_database->tables.end())
	{
		throw std::exception("Table does not exist");
	}
	//create rc
	std::vector<std::string> flds(fields);
	char* rc = create_rc(tb._Ptr, &flds, &values);
	if (!flds.empty())
	{
		throw std::exception(("Field does not exist: " + fields[0]).c_str());
	}
	//create bool map
	bool* bl = create_bool_map(tb._Ptr, &fields);
	//create cnd
	auto cnd = create_cnd(tb._Ptr, where);
	//update
	tb->tree.update(cnd, rc, bl);
}

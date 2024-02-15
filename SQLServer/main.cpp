#include"sql.h"
#include "str.h"
#include"state_graph.h"
#include "sql_graph.h"	
#include <iostream>
#include "database.h"
#include "file_controller.h"
#include "node.h"
#include "bptree.h"
void display(BpTree bt);
int main() {

	//TableField tf1{ "name1", String, 10, "", std::vector<FieldConstrains>{PrimaryKey, NotNull} };
	//Field f1{ tf1 };
	//TableField tf2{ "name1", Int, 0, "", std::vector<FieldConstrains>{PrimaryKey, NotNull} };
	//Field f2{ tf2 };
	//std::vector<Field> fields = std::vector<Field>({ f1, f2 });
	//Record r{ &fields };
	//r.values[0] = "hello";
	//r.values[1] = "1234";
	//auto c = r.encode();
	////
	//Record d{ &fields };
	//d.decode(c);
	////
	//std::cout << d.values[0] << std::endl << d.values[1];

	Storage storage; 
	char* s = new char[100];
	while (1)
	{
		
		std::fill_n(s, 100, ' ');

		std::cout << "|> ";
		std::fgets(s, 100, stdin);

		if (std::string{s} == "exit\n")
			break;

		try {
			auto out = build_sql_graph(s);
			auto sql = static_cast<SQLStatement*>(out->out);
			if (out->is_accept_state)
			{
				sql->storage = &storage;
				sql->check_valid();
				sql->excute();
				std::cout << "done" << std::endl;
			}
		}
		catch (std::exception ex) {
			std::cout << ex.what() << std::endl;
		}

	}
	delete[] s;

	/*std::string sqls[] = {
		"create database db",
		"database db",
		"create table tb(id int, name string 5)",
		"insert into tb(id, name) values('1', 'abc')",
		"insert into tb(id, name) values('2', 'abc')",
		"insert into tb(id, name) values('3', 'abc')",
		"update tb set name values( 'ahm' ) where id = '2'",
		"select name from tb",
		"select name from tb where id = '1'",
		"delete from tb where name = 'ahm'",
		"select name from tb",
		"drop table tb",
		"drop database db"
	};
	Storage storage;
	for (size_t i = 0; i < 13; i++)
	{
		auto& s = sqls[i];
		std::cout << s << std::endl;
		try {
			auto out = build_sql_graph(s);
			auto sql = static_cast<SQLStatement*>(out->out);
			if (out->is_accept_state)
			{
				std::cout << "done" << std::endl;
				sql->storage = &storage;
				sql->check_valid();
				sql->excute();
			}
		}
		catch (std::exception ex) {
			std::cout << ex.what() << std::endl;
		}
	}*/
	
	//const char* fn = "hellotest.txt";
	//FileController* fc = new FileController(fn);
	////fc->open_write();
	//BlockController bc{ fc, true };

	/*char c = 'a';
	char* ar = new char[BLOCKSIZE];
	for (size_t i = 0; i < 10; i++)
	{
		std::fill(ar, ar + BLOCKSIZE, c);
		c++;
		int bn = bc.new_block();
		fc->write_block(ar, bn);
	}*/

	/*auto bd = fc->read_block(1);
	for (auto i = bd; i < bd + BLOCKSIZE; i++)
	{
		std::cout << *bd;
	}*/

	/*bc.erase_block(3);
	std::cout << bc.n_block;*/

	//NodeParamter np{ fc, &bc, 1, 2 };
	//create_rec_node(np, 0).save();
	////fc->close();
	////fc->open_read_write();
	//BpTree bt;
	//bt.param = np;
	//bt.root = 1;
	//char* c = new char[2];
	//for (size_t i = 0; i < 26; i++)
	//{
	//	*c = 'a' + i;
	//	*(c + 1) = '0' + i%10;
	//	bt.insert(c);
	//}

	//*c = 'b';
	////display(bt);
	//bt.remove(c);
	//*c = 'g';
	//bt.remove(c);
	//*c = 'd';
	//bt.remove(c);
	//display(bt);
	/*CharCondition cnd;
	cnd.ccm.push_back(CharCompare{0, 1, GreaterThan, c});
	bt.remove(cnd);
	display(pt)*/

	return 0;
}

void display(BpTree bt) {
	auto it = bt.select_all();
	auto rc = it.next();
	while(rc != nullptr)
	{
		std::cout << *rc << ' ' << *(rc + 1) << std::endl;
		rc = it.next();
	}
}

/*

create database db
1
database db
1
create table tb(id int, name string 5)
1
insert into tb(id, name) values ('12', 'abc')
1
delete from tb where name = 'abc'
1
select name from tb where id = '12'
1
update tb set name values( 'ahm' ) where id = '12'
1
drop table tb
1
drop database db
1

*/
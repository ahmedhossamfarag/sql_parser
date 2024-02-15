#include "sql_graph.h"
#include <initializer_list>
#include "str.h"
using SP = StateParameter*;
using str = std::string;
using namespace std::string_literals;

StateParameter* SelectDatabase::create_object(StateParameter* p)
{
    return new SelectDatabaseStatement();
}

void SelectDatabase::on_init(StateParameter* p)
{
    this->State::on_init(p);

    this->connect(new SpaceTransition(&s1));
    s1.connect(new SpaceTransition(&s1));
    s1.connect(new WordTransion(&s2,
        [](StateParameter* p, str s) {
            static_cast<SelectDatabaseStatement*>(p)->database_name = s;
        })
    );
    s2.connect(new SpaceTransition(&s2));
}

StateParameter* CreateDatabese::create_object(StateParameter* p)
{
    return new CreateDatabaseStatement();
}

void CreateDatabese::on_init(StateParameter* p)
{
    this->State::on_init(p);


    this->connect(new SpaceTransition(&s1));
    s1.connect(new SpaceTransition(&s1));
    s1.connect(new WordTransion(&s2,
        [](StateParameter* p, str s) {
            static_cast<CreateDatabaseStatement*>(p)->database_name = s;
        })
    );
    s2.connect(new SpaceTransition(&s2));
}

StateParameter* CreateTable::create_object(StateParameter* p)
{
    return new CreateTableStatement();
}

void CreateTable::on_init(StateParameter* p)
{
    this->State::on_init(p);

    this->connect(new SpaceTransition(&s1));
    s1.connect(new SpaceTransition(&s1));
    s1.connect(new WordTransion(&s2,
        [](StateParameter* p, str s) {
            static_cast<CreateTableStatement*>(p)->table_name = s;
        })
    );
    s2.connect(new SpaceTransition(&s2));
    s2.connect(new Transion("("s, &s3));
    s3.connect(new SpaceTransition(&s3));
    s3.connect_all({ new WordTransion(&s4, [](SP p, str s)
                    {
                        static_cast<CreateTableStatement*>(p)->fields.push_back(TableField{ s });
                    }
                ), new SpaceTransition(&s3),
        });
    s4.connect(new SpaceTransition(&s5));
    s5.connect_all({ new TuppleTransion(types, 4, &s6, [](SP p, int i) {
        static_cast<CreateTableStatement*>(p)->fields.back().type = static_cast<FieldType>(i);
        }),
        new Transion("string", &s12),
        new SpaceTransition(&s5)
        });
    s6.connect_all({new Transion(")"s, &s7),
        new SpaceTransition(&s8),
        new Transion(",", &s3)
        });
    s7.connect(new SpaceTransition(&s7));
    s8.connect_all({ new Transion(")"s, &s7),
        new SpaceTransition(&s8),
        new Transion(",", &s3),
        new TuppleTransion(constrains, 3, &s6, [](SP p, int i) {
            static_cast<CreateTableStatement*>(p)->fields.back().constrains.push_back(static_cast<FieldConstrains>(i));
            }),
        new Transion("default" , &s9),
        });
    s9.connect(new SpaceTransition(&s10));
    s10.connect_all({ new StringTransion(&s11, [](SP p, str s) {
             static_cast<CreateTableStatement*>(p)->fields.back().defaultValue = s;
            }),
        new SpaceTransition(&s10),
        });
    s11.connect_all({ new Transion(")"s, &s7),
       new SpaceTransition(&s11),
       new Transion(",", &s3)
        });
    s12.connect(new SpaceTransition(&s13));
    s13.connect_all({
        new SpaceTransition(&s13),
        new NumTransion(&s6, [](SP p, str s) {
            static_cast<CreateTableStatement*>(p)->fields.back().type = String;
            static_cast<CreateTableStatement*>(p)->fields.back().field_size = str_to_int(s);
            })
        });
}

StateParameter* DropDatabese::create_object(StateParameter* p)
{
    return new DropDatabaseStatement();
}

void DropDatabese::on_init(StateParameter* p)
{
    this->State::on_init(p);


    this->transions.push_back(new SpaceTransition(&s1));
    s1.transions.push_back(new SpaceTransition(&s1));
    s1.transions.push_back(new WordTransion(&s2,
        [](StateParameter* p, str s) {
            static_cast<DropDatabaseStatement*>(p)->database_name = s;
        })
    );
    s2.transions.push_back(new SpaceTransition(&s2));
}

StateParameter* DropTable::create_object(StateParameter* p)
{
    return new DropTableStatement();
}

void DropTable::on_init(StateParameter* p)
{
    this->State::on_init(p);


    this->transions.push_back(new SpaceTransition(&s1));
    s1.transions.push_back(new SpaceTransition(&s1));
    s1.transions.push_back(new WordTransion(&s2,
        [](StateParameter* p, str s) {
            static_cast<DropTableStatement*>(p)->table_name = s;
        })
    );
    s2.transions.push_back(new SpaceTransition(&s2));
}

StateParameter* Insert::create_object(StateParameter* p)
{
    return new InsertStatement();
}

void Insert::on_init(StateParameter* p)
{
    this->State::on_init(p);
    
    this->connect(new SpaceTransition(&s1));
    s1.connect_all({new Transion("into", &s2),
        new SpaceTransition(&s1)
        });
    s2.connect(new SpaceTransition(&s3));
    s3.connect_all({ new WordTransion(&s4, [](SP p, str s) {
            static_cast<InsertStatement*>(p)->table_name = s;
            }),
        new SpaceTransition(&s3)
        });
    s4.connect_all({ new Transion("(", &s5),
        new SpaceTransition(&s4)
        });
    s5.connect_all({ new WordTransion(&s6, [](SP p, str s) {
            static_cast<InsertStatement*>(p)->fields.push_back(s);
            }),
        new SpaceTransition(&s5)
        });
    s6.connect_all({ new Transion(")", &s7),
        new SpaceTransition(&s6),
        new Transion(",", &s5)
        });
    s7.connect(new SpaceTransition(&s8));
    s8.connect_all({ new Transion("values", &s9),
        new SpaceTransition(&s8)
        });
    s9.connect_all({ new Transion("(", &s10),
       new SpaceTransition(&s9)
        });
    s10.connect_all({ new StringTransion(&s11, [](SP p, str s) {
            static_cast<InsertStatement*>(p)->values.push_back(s);
            }),
        new SpaceTransition(&s10)
        });
    s11.connect_all({ new Transion(")", &s12),
        new SpaceTransition(&s11),
        new Transion(",", &s10)
        });
    s12.connect(new SpaceTransition(&s12));
}

void Where::on_init(StateParameter* p)
{
    this->State::on_init(p);
    

    this->connect(new Transion("where", &s1));
    s1.connect(new SpaceTransition(&s2));
    s2.connect_all({ new WordTransion(&s3, [](SP p, str s) {
            static_cast<ConditionalStatement*>(p)->where.expresion.push_back(BooleanExpresion{s});
            }),
        new SpaceTransition(&s5)
        });
    s3.connect_all({ new TuppleTransion(m, 6, &s4, [](SP p, int i) {
        static_cast<ConditionalStatement*>(p)->where.expresion.back().compare = static_cast<BooleanCompare>(i);
        }),
        new SpaceTransition(&s3)
        });
    s4.connect_all({ new StringTransion(&s5, [](SP p, str s) {
            static_cast<ConditionalStatement*>(p)->where.expresion.back().value = s;
            }),
        new SpaceTransition(&s4)
        });
    s5.connect_all({ new TuppleTransion(l, 2, &s2, [](SP p, int i) {
        static_cast<ConditionalStatement*>(p)->where.operators.push_back(static_cast<BooleanOperators>(i));
        }),
        new SpaceTransition(&s5)
        });
}

StateParameter* Select::create_object(StateParameter* p)
{
    return new SelectStatement();
}

void Select::on_init(StateParameter* p)
{
    this->State::on_init(p);


    w.is_accept_state = true;

    this->transions.push_back(new SpaceTransition(&s1));
    s1.connect_all({ new SpaceTransition(&s1),
        new WordTransion(&s2,
        [](StateParameter* p, str s) {
            static_cast<SelectStatement*>(p)->fields.push_back(s);
        })
        });
    s2.connect_all({new SpaceTransition(&s3),
        new Transion(",", &s1)
        });
    s3.connect_all({ new SpaceTransition(&s3),
        new Transion(",", &s1),
        new Transion("from", &s4)
        });
    s4.connect(new SpaceTransition(&s5));
    s5.connect_all({ new WordTransion(&s6,
        [](StateParameter* p, str s) {
            static_cast<SelectStatement*>(p)->table_name = s;
        }), new SpaceTransition(&s5)
        });
    s6.connect(new SpaceTransition(&w));
    w.connect(new SpaceTransition(&w));
}

StateParameter* Delete::create_object(StateParameter* p)
{
    return new DeleteStatement();
}

void Delete::on_init(StateParameter* p)
{
    this->State::on_init(p);

    w.is_accept_state = true;

    this->transions.push_back(new SpaceTransition(&s1));
    s1.connect_all({ new SpaceTransition(&s1),
        new Transion("from", &s2)
        });
    s2.connect(new SpaceTransition(&s3));
    s3.connect_all({ new WordTransion(&s4,
        [](StateParameter* p, str s) {
            static_cast<DeleteStatement*>(p)->table_name = s;
        }), new SpaceTransition(&s3)
        });
    s4.connect(new SpaceTransition(&w));
    w.connect(new SpaceTransition(&w));

}

StateParameter* Update::create_object(StateParameter* p)
{
    return new UpdateStatement();
}

void Update::on_init(StateParameter* p)
{
    this->State::on_init(p);

    w.is_accept_state = true;

    this->connect(new SpaceTransition(&s1));
    s1.connect_all({ new SpaceTransition(&s1),
        new WordTransion(&s2,
        [](StateParameter* p, str s) {
            static_cast<UpdateStatement*>(p)->table_name = s;
        })
        });
    s2.connect(new SpaceTransition(&s3));
    s3.connect_all({ new Transion("set", &s4),
        new SpaceTransition(&s3)
        });
    s4.connect(new SpaceTransition(&s5));
    s5.connect_all({ new SpaceTransition(&s5),
        new WordTransion(&s6,
        [](StateParameter* p, str s) {
            static_cast<UpdateStatement*>(p)->fields.push_back(s);
        })
        });
    s6.connect_all({ new SpaceTransition(&s7),
        new Transion(",", &s5)
        });
    s7.connect_all({ new SpaceTransition(&s7),
        new Transion(",", &s5),
        new Transion("values", &s8)
        });
    s8.connect_all({ new Transion("(", &s9),
       new SpaceTransition(&s8)
        });
    s9.connect_all({ new StringTransion(&s10, [](SP p, str s) {
            static_cast<UpdateStatement*>(p)->values.push_back(s);
            }),
        new SpaceTransition(&s9)
        });
    s10.connect_all({ new Transion(")", &s11),
        new SpaceTransition(&s10),
        new Transion(",", &s9)
        });
    s11.connect(new SpaceTransition(&w));
    w.connect(new SpaceTransition(&w));
}

void SQL::on_init(StateParameter* p)
{
    this->State::on_init(p);

    this->connect_all({
        new Transion("create", &s1),
        new Transion("drop", &s3),
        new Transion("database", &db),
        new Transion("insert", &in),
        new Transion("select", &se),
        new Transion("delete", &de),
        new Transion("update", &up)
        });
    s1.connect(new SpaceTransition(&s2));
    s2.connect_all({
        new Transion("database", &cdb),
        new Transion("table", &ct),
        new SpaceTransition(&s2)
        });
    s3.connect(new SpaceTransition(&s4));
    s4.connect_all({
        new Transion("database", &ddb),
        new Transion("table", &dt),
        new SpaceTransition(&s4)
        });
}

State* build_sql_graph(std::string statement)
{
    StatementParser parser{ statement };
    SQL sql;
    sql.on_init(nullptr);
    return sql.excute(&parser);
}

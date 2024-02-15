#include "sql_parse.h"

std::vector<Database>::iterator get_db(Storage* storage, std::string name)
{
    return find_if<Database>(&storage->databases, [=](Database db) {return db.database_name == name; });
}

bool file_exist(std::string name)
{
    FILE* fl = nullptr;
    fopen_s(&fl, name.c_str(), "r");
    return fl != NULL;
}

bool db_exist(Storage* storage, std::string name)
{
    return get_db(storage,  name) != storage->databases.end();
}

void create_fl(std::string name)
{
    FILE* fl = nullptr;
    fopen_s(&fl, name.c_str(), "w");
    if (!fl)
    {
        throw std::exception("Can't create file");
    }
    char* ar = create_blk();
    fwrite(ar, sizeof(char), BLOCKSIZE, fl);
    fclose(fl);
}

void write_db(Database* db)
{
    char* blk = db->file_cntr->read_block(0);
    db->write_to(blk + DBstart);
    db->file_cntr->write_block(blk, 0);
}

void read_db(Database* db)
{
    char* blk = db->file_cntr->read_block(0);
    db->read_from(blk + DBstart);
}

bool tb_exist(Database* db, std::string name)
{
    return get_tb(db, name) != db->tables.end();
}

std::vector<Field> create_fields(std::vector<TableField>* tf)
{
    std::vector<Field> vc;
    for (auto pt = tf->begin(); pt < tf->end(); pt++)
    {
        vc.push_back(Field(*pt));
    }
    return vc;
}

void format_fields(std::vector<Field>* fl)
{
    auto gr = split<Field>(fl, [](Field f) {return f.is_key(); });
    auto v1 = gr._Myfirst._Val;
    auto v2 = gr._Get_rest()._Myfirst._Val;
    std::copy(v1.begin(), v1.end(), fl->begin());
    std::copy(v2.begin(), v2.end(), fl->begin() + v1.size());
}

void create_tr(Database* db, Table* tb)
{
    NodeParamter np{ db->file_cntr, db->blk_cntr, tb->key_size(), tb->record_size() };
    tb->tree.param = np;
    auto st = create_rec_node(np, 0);
    st.save();
    tb->tree.root = st.blck_indx;
}

std::vector<Table>::iterator get_tb(Database* db, std::string name)
{
    return find_if<Table>(&db->tables, [=](Table t) {return t.table_name == name; });
}

std::map<std::vector<Field>::iterator, std::string> map_fl_vl(std::vector<Field>* fl, std::vector<std::string>* flds, std::vector<std::string>* vls)
{
    std::map<std::vector<Field>::iterator, std::string> m;
    for (auto pt = fl->begin(); pt < fl->end(); pt++)
    {
        auto ps = find_if<std::string>(flds, [=](std::string s) {return pt->tfield.field_name == s; });
        if (ps == flds->end())
        {
            m[pt] = "";
        }
        else
        {
            auto vl = vls->begin() + (ps - flds->begin());
            m[pt] = *vl;
            flds->erase(ps);
            vls->erase(vl);
        }
    }
    return m;
}

char* create_rc(Table* tb, std::vector<std::string>* flds, std::vector<std::string>* vls)
{
    char* rc = new char[tb->record_size()];
    auto m = map_fl_vl(&tb->fields, flds, vls);
    char* at = rc;
    for (auto pt = m.begin(); pt != m.end(); pt++)
    {
        pt->first->encode(pt->second, at);
        at += pt->first->tfield.field_size;
    }
    return rc;
}

void display(Table* tb, char* c)
{
    for (auto pt = tb->fields.begin(); pt < tb->fields.end(); pt++)
    {
        std::cout << pt->decode(c) << '\t';
        c += pt->tfield.field_size;
    }
    std::cout << std::endl;
}

bool* create_bool_map(Table* tb, std::vector<std::string>* flds)
{
    int n = tb->record_size();
    bool* bl = new bool[n];
    std::fill_n(bl, n, 0);
    bool* at = bl;
    for (auto pt = tb->fields.begin(); pt < tb->fields.end(); pt++)
    {
        auto ps = find_if<std::string>(flds, [=](std::string s) {return s == pt->tfield.field_name; });
        if (ps != flds->end())
        {
            std::fill_n(at, pt->tfield.field_size, 1);
            flds->erase(ps);
        }
        at += pt->tfield.field_size;
    }
    return bl;
}

std::vector<Field>::iterator get_fld(Table* tb, std::string name)
{
    return find_if<Field>(&tb->fields, [=](Field f) {return name == f.tfield.field_name; });
}

int get_fld_ps(Table* tb, std::vector<Field>::iterator fld)
{
    int i = 0;
    for (auto pt = tb->fields.begin(); pt < fld; pt++)
    {
        i += pt->tfield.field_size;
    }
    return i;
}

CharCondition create_cnd(Table* t, WhereClause wh)
{
    std::vector<CharCompare> cm;
    for (auto pt = wh.expresion.begin(); pt < wh.expresion.end(); pt++)
    {
        auto f = get_fld(t, pt->field);
        int n = f->tfield.field_size;
        char* c = new char[n];
        f->encode(pt->value, c);
        int i = get_fld_ps(t, f);
        cm.push_back(CharCompare{ i, n, pt->compare, c });
    }
    return CharCondition{ cm, wh.operators };
}


#include "bptree.h"
#include <iostream>

void BpTree::set_root(int ndx)
{
    root = ndx;
    //param.file->go_to(root_ps);
    //param.file->write_int8(ndx);
}

RecordNode* BpTree::get_first_rec_blck()
{
    int ndx = root;
    Node* nd;
    while (true)
    {
        nd = read_node(param, ndx);
        
        if (nd->is_rec_node())
            break;
        else
            ndx = *static_cast<IndexNode*>(nd)->indx_at(0);
    }
    return static_cast<RecordNode*>(nd);
}

void insertEx(Node* nd, char* rec)
{
    if (nd->is_rec_node())
    {
        static_cast<RecordNode*>(nd)->insert(rec);
    }
    else
    {
        auto xn = static_cast<IndexNode*>(nd);
        int i = xn->search(rec);
        int ndx = *xn->indx_at(i);
        auto nnd = read_node(nd->param, ndx);
        insertEx(nnd, rec);
        if (nnd->isfull()) 
        {
            auto nk = nnd->split();
            xn->insert(nk);
        }
    }
}

void BpTree::insert(char* rec)
{
    auto nd = read_node(param, root);
    insertEx(nd, rec);
    if (nd->isfull())
    {

        auto nk = nd->split();
        auto nn = create_indx_node(param);
        nn.set_frst_indx(nd->blck_indx);
        nn.insert(nk);
        set_root(nn.blck_indx);
    }

}

void removeEx(Node* nd)
{
    if (!nd->is_rec_node())
    {
        auto xn = static_cast<IndexNode*>(nd);
        for (char* i = xn->indx_at(0); i <= xn->indx_at(xn->seq_size); i += xn->param.key_size + BlckPtrSz)
        {
            removeEx(read_node(nd->param, *i));
        }
    }
    nd->param.block_cntr->erase_block(nd->blck_indx);
}

void BpTree::remove_all()
{
    removeEx(read_node(param, root));
    auto nn = create_rec_node(param, 0);
    nn.save();
    set_root(nn.blck_indx);
}

void removeEx(Node* nd, char* key)
{
    if (nd->is_rec_node())
    {
        static_cast<RecordNode*>(nd)->remove(key);
    }
    else
    {
        auto xn = static_cast<IndexNode*>(nd);
        int i = xn->search(key);
        int ndx = *xn->indx_at(i);
        auto nnd = read_node(nd->param, ndx);
        removeEx(nnd, key);
        if (nnd->can_join())
        {
            if (i != xn->seq_size)
            {
                auto nxn = read_node(nd->param, *xn->indx_at(i + 1));
                if (nnd->can_join(nxn))
                {
                    auto ky = xn->key_at(i);
                    nnd->join_left(nxn, ky);
                    xn->remove(ky);
                }
            }
            if (i != 0)
            {

                auto prn = read_node(nd->param, *xn->indx_at(i - 1));
                if (nnd->can_join(prn))
                {
                    auto ky = xn->key_at(i - 1);
                    prn->join_left(nnd, ky);
                    xn->remove(ky);
                }
            }
            
        }
    }
}

void BpTree::remove(char* key)
{
    auto nd = read_node(param, root);
    removeEx(nd, key);
    if (!nd->is_rec_node() && nd->seq_size == 0)
    {
        set_root(*static_cast<IndexNode*>(nd)->indx_at(0));
        param.block_cntr->erase_block(nd->blck_indx);
    }
}

void removeEx(Node* nd, CharCondition cnd) 
{
    if (nd->is_rec_node())
    {
        static_cast<RecordNode*>(nd)->remove(cnd);
    }
    else
    {
        auto xn = static_cast<IndexNode*>(nd);

        for (size_t i = 0; i <= xn->seq_size; i++)
        {
            auto nnd = read_node(nd->param, *xn->indx_at(i));
            removeEx(nnd, cnd);
        }

        Node* pnd = read_node(nd->param, *xn->indx_at(0));
        int indx = 1;
        bool b = true;
        while (b)
        {
            b = false;
            for (size_t i = indx; i <= xn->seq_size; i++)
            {
                auto nnd = read_node(nd->param, *xn->indx_at(i));
                if (pnd->can_join(nnd))
                {
                    auto ky = xn->key_at(i - 1);
                    pnd->join_left(nnd, ky);
                    xn->remove(ky);
                    b = true;
                    indx = i;
                    break;
                }
                pnd = nnd;
            }
        }
    }
}

void BpTree::remove(CharCondition cnd)
{
    auto nd = read_node(param, root);
    removeEx(nd, cnd);
    if (!nd->is_rec_node() && nd->seq_size == 0)
    {
        set_root(*static_cast<IndexNode*>(nd)->indx_at(0));
        param.block_cntr->erase_block(nd->blck_indx);
    }
}

void updateEx(char* old, char* nw, bool* at, int ln)
{
    for (size_t i = 0; i < ln; i++)
    {
        if (*at)
        {
            *old = *nw;
        }
        old++;
        nw++;
        at++;
    }
}

void BpTree::update_all(char* rec, bool* at)
{
    update(CharCondition(), rec, at);
}

void BpTree::update(CharCondition cnd, char* rec, bool* at)
{
    auto rcn = get_first_rec_blck();
    Iterator itr{ rcn };
    char* c;

    while (true)
    {
        c = itr.next();

        if (c == nullptr)
        {
            rcn->save();
            return;
        }

        if (itr.nd != rcn)
        {
            rcn->save();
            rcn = itr.nd;
        }
        if(cnd.operate_on(c))
            updateEx(c, rec, at, param.record_size);
    }
}

std::tuple<int,int> select_limit(Node* nd, char* key)
{
    if (nd->is_rec_node())
    {
        int i = static_cast<RecordNode*>(nd)->search_upper(key);
        return std::make_tuple(nd->blck_indx, i);
    }
    else
    {
        auto xn = static_cast<IndexNode*>(nd);
        int i = xn->search(key);
        int ndx = *xn->indx_at(i);
        auto nnd = read_node(nd->param, ndx);
        return select_limit(nnd, key);
    }
}

char* selectEx(Node* nd, char* key)
{
    if (nd->is_rec_node())
    {
        auto rcn = static_cast<RecordNode*>(nd);
        int i = rcn->search(key);
        if (i == -1)
            return nullptr;
        return rcn->record_at(i);
    }
    else
    {
        auto xn = static_cast<IndexNode*>(nd);
        int i = xn->search(key);
        int ndx = *xn->indx_at(i);
        auto nnd = read_node(nd->param, ndx);
        return selectEx(nnd, key);
    }
}

char* BpTree::select(char* key)
{
    return selectEx(read_node(param, root), key);
}

Iterator BpTree::select_all()
{
    return select(CharCondition());
}

Iterator BpTree::select(CharCondition cnd)
{
    auto rcn = get_first_rec_blck();
    Iterator itr{ rcn };
    itr.cnd = cnd;
    return itr;
}

Iterator::Iterator(RecordNode* nd) : nd(nd), i(0),
    at(nd->first_key()), cnd(CharCondition()),
    end_nd_indx(0), end_indx(-1)
{
}

char* Iterator::_next()
{
    if (end_nd_indx != 0 && nd->blck_indx == end_nd_indx)
    {
        if (end_indx == -1 && i == nd->seq_size)
            return nullptr;
        if (end_indx != -1 && i == end_indx)
            return nullptr;
    }

    if (i == nd->seq_size)
    {
        if(nd->next_node == 0)
            return nullptr;
        else
        {
            nd = static_cast<RecordNode*>(read_node(nd->param, nd->next_node));
            set(0);
        }
        return _next();
    }

    char* c = at;
    at += nd->param.record_size;
    i++;
    return c;
}

char* Iterator::next()
{
    char* c;

    do
    {
        c = _next();
    } while (c != nullptr && !cnd.operate_on(c));

    return c;
}

void Iterator::set(int i)
{
    this->i = i;
    at = nd->first_key() + i * nd->param.record_size;
}

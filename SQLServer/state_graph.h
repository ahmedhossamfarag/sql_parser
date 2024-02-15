#pragma once
#include <vector>
#include <functional>
#include <string>


class StatementParser
{
public: 
	std::string statement;

	StatementParser(std::string statement);
	bool test(std::string value);
	bool test_tupple(std::string values[], size_t size);
	bool test_word();
	bool test_num();
	bool test_string();
	std::string read(std::string value);
	int read_tupple(std::string values[], size_t size);
	std::string read_word();
	std::string read_num();
	std::string read_string();

	bool get_done();
	__declspec(property(get = get_done)) bool is_done;
};


class StateParameter {
};

class Transion;

class State
{
public:
	std::vector<Transion*> transions;
	StateParameter* out;
	bool is_accept_state;

	State(bool is_accept_state = false);
	virtual StateParameter* create_object(StateParameter* out);
	virtual void on_init(StateParameter* out);
	virtual void connect(Transion* t); 
	void connect_all(std::initializer_list<Transion*> l);
	virtual State* excute(StatementParser* parser);
};

class Transion
{
public:
	std::string value;
	State* next;
	std::function<void(StateParameter*, std::string)> update;

	Transion(std::string value, State* next, std::function<void(StateParameter*, std::string)> f = [](StateParameter* p, std::string s) {});
	virtual bool test(StatementParser* parser);
	virtual std::string read(StatementParser* parser);
	virtual void cross(StatementParser* parser, StateParameter* out);

};

class SpaceTransition : public Transion
{
public:
	SpaceTransition(State* next);
};

class TuppleTransion : public Transion
{
public:
	std::string* values;
	size_t size;
	std::function<void(StateParameter*, int)> update_t;

	TuppleTransion(std::string* values, size_t size, State* next, std::function<void(StateParameter*,int)> f);
	virtual bool test(StatementParser* parser) override;
	virtual void cross(StatementParser* parser, StateParameter* out) override;
};

class WordTransion : public Transion
{
public:

	WordTransion(State* next, std::function<void(StateParameter*, std::string)> f);
	virtual bool test(StatementParser* parser) override;
	virtual std::string read(StatementParser* parser) override;
};

class NumTransion : public Transion
{
public:

	NumTransion(State* next, std::function<void(StateParameter*, std::string)> f);
	virtual bool test(StatementParser* parser) override;
	virtual std::string read(StatementParser* parser) override;
};



class StringTransion : public Transion
{
public:

	StringTransion(State* next, std::function<void(StateParameter*, std::string)> f);
	virtual bool test(StatementParser* parser) override;
	virtual std::string read(StatementParser* parser) override;
};
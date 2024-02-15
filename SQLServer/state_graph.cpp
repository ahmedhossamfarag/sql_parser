#include "state_graph.h"
#include "sql.h"
#include "str.h"
#include <algorithm>
#include <iostream>

StatementParser::StatementParser(std::string statement)
{
	this->statement = statement;
}

bool StatementParser::test(std::string value)
{
	return str_start_with_i(statement, value);
}

bool StatementParser::test_tupple(std::string values[], size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		if (test(values[i]))
			return true;
	}
	return false;
}

bool StatementParser::test_word()
{
	return char_is_alpha(statement[0]);
}

bool StatementParser::test_num()
{
	return char_is_num(statement[0]);
}

bool StatementParser::test_string()
{
	return statement[0] == '\'' && statement.find('\'', 1) > 0;
}

std::string StatementParser::read(std::string value)
{
	statement = statement.substr(value.length());
	return value;
}

int StatementParser::read_tupple(std::string values[], size_t size)
{
	for (int i = 0; i < size; i++)
	{
		if (test(values[i]))
		{
			read(values[i]);
			return i;
		}
	}
	return -1;
}


std::string StatementParser::read_word()
{
	size_t i = 0;
	for (i = 0; i < statement.length(); i++)
	{
		if (!char_is_word(statement[i]))
		{
			break;
		}
	}
	auto s = statement.substr(0, i);
	statement = statement.substr(i);
	return s;
}

std::string StatementParser::read_num()
{
	size_t j = 0;
	for (size_t i = 0; i < statement.length(); i++)
	{
		if (!char_is_num(statement[i]))
		{
			j = i;
			break;
		}
	}
	auto s = statement.substr(0, j);
	statement = statement.substr(j);
	return s;
}

std::string StatementParser::read_string()
{
	size_t i = statement.find('\'', 1);
	auto s = statement.substr(1, i-1);
	statement = statement.substr(i + 1);
	return s;
}

bool StatementParser::get_done()
{
	return statement.empty() || statement == "\n";
}

State::State(bool is_accept_state)
{
	this->is_accept_state = is_accept_state;
	this->out = nullptr;
}

StateParameter* State::create_object(StateParameter* out)
{
	return out;
}

void State::on_init(StateParameter* out)
{
	this->out = create_object(out);
}

void State::connect(Transion* t)
{
	this->transions.push_back(t);
}

void State::connect_all(std::initializer_list<Transion*> l)
{
	this->transions.insert(transions.end(), l.begin(), l.end());
}

State* State::excute(StatementParser* parser)
{
	if (parser->is_done)
	{
		return this;
	}

	auto t = std::find_if(transions.begin(), transions.end(), [parser](Transion* tr) {return tr->test(parser); });

	if (t != transions.end()) {
		auto trans = *t;
		trans->cross(parser, this->out);
		trans->next->on_init(this->out);
		return trans->next->excute(parser);
	}
	else
	{
		throw std::exception(("UnExpected Token: " + parser->statement).c_str());
	}

}

Transion::Transion(std::string value, State* next, std::function<void(StateParameter*, std::string)> f)
{
	this->value = value;
	this->next = next;
	this->update = f;
}

bool Transion::test(StatementParser* parser)
{
	return parser->test(this->value);
}

std::string Transion::read(StatementParser* parser)
{
	return parser->read(this->value);
}

void Transion::cross(StatementParser* parser, StateParameter* out)
{
	this->update(out, this->read(parser));
}

SpaceTransition::SpaceTransition(State* next) : Transion(" ", next)
{
}

TuppleTransion::TuppleTransion(std::string* values, size_t size, State* next, std::function<void(StateParameter*, int)> f) : Transion("\\T", next)
{
	this->values = values;
	this->size = size;
	this->update_t = f;
}

bool TuppleTransion::test(StatementParser* parser)
{
	return parser->test_tupple(this->values, this->size);
}

void TuppleTransion::cross(StatementParser* parser, StateParameter* out)
{
	this->update_t(out, parser->read_tupple(this->values, this->size));
}

WordTransion::WordTransion(State* next, std::function<void(StateParameter*, std::string)> f) : Transion("\\W", next, f)
{
}

bool WordTransion::test(StatementParser* parser)
{
	return parser->test_word();
}

std::string WordTransion::read(StatementParser* parser)
{
	return parser->read_word();
}

NumTransion::NumTransion(State* next, std::function<void(StateParameter*, std::string)> f) : Transion("\\N", next, f)
{
}

bool NumTransion::test(StatementParser* parser)
{
	return parser->test_num();
}

std::string NumTransion::read(StatementParser* parser)
{
	return parser->read_num();
}

StringTransion::StringTransion(State* next, std::function<void(StateParameter*, std::string)> f) : Transion("\\S", next, f)
{
}

bool StringTransion::test(StatementParser* parser)
{
	return parser->test_string();
}

std::string StringTransion::read(StatementParser* parser)
{
	return parser->read_string();
}

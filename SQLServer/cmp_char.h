#pragma once
#include "str.h"
#include <vector>

enum BooleanOperators
{
	And,
	Or
};

enum BooleanCompare
{
	GreaterThanOrEqual,
	LessThanOrEqual,
	NotEqual,
	Equal,
	GreaterThan,
	LessThan,
};

struct BooleanExpresion
{
	std::string field;
	BooleanCompare compare;
	std::string value;
};

struct WhereClause
{
	std::vector<BooleanOperators> operators;
	std::vector<BooleanExpresion> expresion;
};

struct CharCompare
{
	int i;
	int n;
	BooleanCompare cmp; char* c;
};

struct CharCondition
{
	std::vector<CharCompare> ccm;
	std::vector<BooleanOperators> ops;
	
	bool operate_on(char* rc);
};
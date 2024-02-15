#include "cmp_char.h"

bool mix(std::vector<bool> bl, std::vector<BooleanOperators> ops)
{
	bool b = bl[0];
	auto bp = bl.begin() + 1;
	for (auto op = ops.begin(); op < ops.end(); op++)
	{
		if (*op == And)
		{
			b &= *bp;
		}
		else
		{
			b |= *bp;
		}
		bp++;
	}
	return b;
}

bool cmp_char(char* rc, CharCompare ccm)
{
	int i = compare_char(rc + ccm.i, ccm.c, ccm.n);
	switch (ccm.cmp)
	{
	case GreaterThanOrEqual:
		return i >= 0;
	case LessThanOrEqual:
		return i <= 0;
	case NotEqual:
		return i != 0;
	case GreaterThan:
		return i > 0;
	case LessThan:
		return i < 0;
	case Equal:
		return i == 0;
	default:
		return true;
	}
}

std::vector<bool> cmp_char(char* rc, std::vector<CharCompare> ccm)
{
	std::vector<bool> bl;
	for (auto cm = ccm.begin(); cm < ccm.end(); cm++)
	{
		bl.push_back(cmp_char(rc, *cm));
	}
	return bl;
}

bool CharCondition::operate_on(char* rc)
{
	if (ccm.empty())
		return true;
	auto b = cmp_char(rc, ccm);
	return mix(b, ops);
}

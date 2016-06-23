#include "Common.h"

namespace QuickDef
{
	Player& QuickDef::operator++ (Player& lhs)// for simplification of player switching
	{
		if (lhs == 1)
		{
			lhs = Black;
			return lhs;
		}
		else
		{
			lhs = Red;
			return lhs;
		}
	};
}

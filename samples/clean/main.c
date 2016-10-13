#include "xmlclean.h"

int main()
{
	Parser p = { 0 };
	parse(&p);
	done(&p);
	return 0;
}

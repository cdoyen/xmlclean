#include "xmlclean.h"
#include <stdio.h>

int main(int argc, char**argv)
{
	FILE *f = fopen(argv[1], "rb");
	FILE *fo = fopen(argv[2], "wb");
	if (!f) return perror(argv[1]), 1;
	if (!fo) return fclose(f), perror(argv[2]), 1;
	{
		Parser p = { 0 };
		p.inputcbdata = f;
		p.outputcbdata = fo;
		p.worker = worker_csv;

		parse(&p);

		fclose(fo);
		fclose(f);
		done(&p);
	}
	return 0;
}

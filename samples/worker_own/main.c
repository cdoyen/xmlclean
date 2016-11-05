#include "xmlclean.h"
#include <stdio.h> /* fopen,fclose,fprintf,stderr */

int worker_own(int typ, const unsigned char *tag, size_t taglen, int out(), void* f, Parser *p)
{
	switch (typ)
	{
	case NORMALCLOSE_:
		break;
	case OPENTAG_:
	case SELFCLOSE_:
	{
		Memblock m = getattribut(tag, taglen, (Memblock) { 2, "id" });
		if (m.b)
			out(f, m.b, m.z);
		break;
	}
	case FRAMECLOSE_:
		break;
	case COMMENT_:
		break;
	case PROLOG_:
		break;
	case UNKNOWN_:
		break;
	}
	return XML_OK;
}

int main(int argc, char**argv)
{
	FILE *f = fopen(argv[1], "rb");
	if (!f) return perror(argv[1]), 1;
	{
		Parser p = { f, 0, worker_own, 0, writeln };
		{
			int r = parse(&p);
			if (r)
			{
				if (r == ERRMEM)
					fprintf(stderr, "Fehler Speichermangel");
				else
					if (r == ERRHIERAR)
						fprintf(stderr, "Fehler Hierarchie");
					else
						fprintf(stderr, "Fehler Output");
			}

			fclose(f);

			done(&p);
			return r;
		}
	}
}

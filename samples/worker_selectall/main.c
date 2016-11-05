#include "xmlclean.h"
#include <stdio.h> /* fopen,fclose,fprintf,stderr */

int worker_selectall(int typ, const unsigned char *tag, size_t taglen, int out(), void* f, Parser *p)
{
	switch (typ)
	{
	case NORMALCLOSE_:
		break;
	case OPENTAG_:
		break;
	case SELFCLOSE_:
		break;
	case FRAMECLOSE_:
		break;
	case COMMENT_:
		break;
	case PROLOG_:
		break;
	case UNKNOWN_:
		break;
	}

	out(f, p->content, p->contentlen);
	out(f, "\t:\t", 3);
	out(f, tag, taglen);
	out(f, "\n", 1);

	return XML_OK;
}

int main(int argc, char**argv)
{
	FILE *f = fopen(argv[1], "rb");
	if (!f) return perror(argv[1]), 1;
	{
		Parser p = { f, 0, worker_selectall };
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

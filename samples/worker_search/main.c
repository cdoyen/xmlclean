#include "xmlclean.h"
#include <stdio.h> /* fopen,fclose,fprintf,stderr */

int worker_search(int typ, const unsigned char *tag, size_t taglen, int out(), void* f, Parser *p)
{
	switch (typ)
	{
	case NORMALCLOSE_:
	{
		const Memblock *pattern = p->userdata;
		if (anymatch(pattern->b, pattern->z, tag + 1, taglen - 1))  /* führendes '/' beim closetag ignorieren mit +1/-1 */
		{
			out(f, p->content, p->contentlen);
		}
	}
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
	return XML_OK;
}

int main(int argc, char**argv)
{
	FILE *f = fopen(argv[1], "rb");
	if (!f) return perror(argv[1]), 1;
	{
		Parser p = { 0 };
		init_light(&p, f, 0, worker_search, 0, writeln, &(Memblock) { 20, "name|*capital*|*tude" }, 0, 0, 0);
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

#include "xmlclean.h"
#include <stdio.h> /* fopen,fclose,fprintf,stderr */


int main(int argc, char**argv)
{
	FILE *f = fopen(argv[1], "rb");
	if (!f) return perror(argv[1]), 1;
	{
		Parser p = { 0 };
		init_light(&p, f, 0, worker_nop, 0, 0, 0, 0, 0, 0);
		{
			int r = parse_light(&p);
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

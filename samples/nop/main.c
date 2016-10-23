#include "xmlclean.h"
#include <stdio.h> /* fopen,fclose,fprintf,stderr */


int main(int argc, char**argv)
{
	FILE *f = fopen(argv[1], "rb");
	if (!f) return perror(argv[1]), 1;
	{
		Parser p = { f, 0, worker_nop };
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

			fprintf(stderr, "\nebenemax: %lu\ntag: %lu\nframetag: %lu\nselfclose: %lu\ncomment: %lu\nprolog: %lu\nall-tags: %lu\ncontent_min: %lu\ncontent_max: %lu\ntag_min: %lu\ntag_max: %lu\nibytes: %lu\nobytes: %lu\nratio: %.2f%%\n",
				(unsigned long)p.stat.ebenemax,
				(unsigned long)p.stat.tag,
				(unsigned long)p.stat.frametag,
				(unsigned long)p.stat.selfclose,
				(unsigned long)p.stat.comment,
				(unsigned long)p.stat.prolog,
				(unsigned long)(p.stat.comment + p.stat.selfclose + p.stat.frametag + p.stat.tag),
				(unsigned long)p.stat.content_min,
				(unsigned long)p.stat.content_max,
				(unsigned long)p.stat.tag_min,
				(unsigned long)p.stat.tag_max,
				(unsigned long)p.stat.ibytes,
				(unsigned long)p.stat.obytes,
				p.stat.ibytes ? p.stat.obytes*100. / p.stat.ibytes : 0);
			fclose(f);

			done(&p);
			return r;
		}
	}
}

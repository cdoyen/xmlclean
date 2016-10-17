/******************************************************************************
Copyright (C) V1.0 2016  cdoyen@github

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

******************************************************************************/

#define _FILE_OFFSET_BITS  64  /* gcc,clang,mingw support for Files > 4GB on 32Bit also */

#include "xmlclean.h"

#include <stdio.h>  /* fread,fwrite,fpos_t,stdin,stdout */
#include <stdlib.h> /* realloc,free */
#include <string.h> /* memchr,memmove,memcpy */

static size_t readblock(FILE *f, unsigned char*s, size_t w)
{
	fpos_t fp;
	if (fgetpos(f, &fp), 1 == fread(s, w, 1, f))
	{
		return w;
	}
	fsetpos(f, &fp);
	return fread(s, 1, w, f);
}

static size_t writeblock(FILE *f, const unsigned char*s, size_t w)
{
	return fwrite(s, w, 1, f) == 1 ? w : 0;
}

static const unsigned char *memchrignore(const unsigned char *y, size_t w)
{  /* für <![CDATA...]...> Zeugs */
	int in = 0;
	while (w--)
	{
		if (*y == '[')
			++in;
		else if (*y == ']')
			--in;
		else if (*y == '>')
		{
			if (!in)
				return y;
		}
		++y;
	}
	return 0;
}

static void countAttributes(const unsigned char* b, size_t z, size_t *c)
{
	int in = 0, in1 = 0;
	size_t i;
	for (i = 0; i < z; ++i)
	{
		if (b[i] == '"')
		{
			if (!in1)
				in ^= 1;
		}
		else
			if (b[i] == '=')
			{
				if (!in && !in1)
					++*c;
			}
			else
				if (b[i] == '\'')
				{
					if (!in)
						in1 ^= 1;
				}
	}
}

static int addPath(const unsigned char *tag, size_t taglen, size_t ebene, Path *pt)
{
	if (ebene + 1 > pt->bz)
	{
		pt->List = realloc(pt->List, (pt->bz = ebene + 1) * sizeof*pt->List);
		if (!pt->List) return ERRMEM;
		pt->List[ebene].b = 0; pt->List[ebene].z = 0;
	}
	if (pt->List[ebene].z < taglen)
	{
		pt->List[ebene].b = realloc(pt->List[ebene].b, taglen);
		if (!pt->List[ebene].b) return ERRMEM;
	}
	memcpy(pt->List[ebene].b, tag, pt->List[ebene].z = taglen);
	return 0;
}

static int nextpair(const unsigned char **value, size_t *valuelen, const unsigned char **tag, size_t *taglen, MMan *mm, size_t cb(), void *f)
{
	const unsigned char *s;
	size_t start = mm->i;

	while (!(s = memchr(mm->b + mm->i, '<', mm->g)))
	{
		if (start)
		{
			memmove(mm->b, mm->b + mm->i, mm->g);
			mm->i = mm->g;
			start = 0;
			mm->g = 0;
		}
		mm->i += mm->g;
		if (mm->i == mm->e)
		{
			mm->b = realloc(mm->b, mm->e += mm->buf);
			if (!mm->b) return ERRMEM;

			mm->g = mm->buf;
		}
		else
			mm->g = mm->e - mm->i;

		mm->g = cb(f, mm->b + mm->i, mm->g);
		if (!mm->g)
		{
			*value = mm->b + start;
			*valuelen = mm->i;
			*taglen = 0;
			*tag = (unsigned char*)"";
			return ERREND;
		}
	}
	*valuelen = (size_t)(s - (mm->b + start));
	mm->g -= ((size_t)(s - mm->b) + 1) - mm->i;
	mm->i = (size_t)(s - mm->b) + 1;

	{
		size_t i = 0;
		int first = 1;
		while (mm->i == mm->e || ((first = (first ? (i = (mm->b[mm->i] == '!') ? mm->i : 0, 0) : 0)),
			!(s = (i ? memchrignore(mm->b + i, mm->g + (mm->i - i)) : memchr(mm->b + mm->i, '>', mm->g)))))
		{
			mm->i += mm->g;
			if (mm->i == mm->e)
			{
				mm->b = realloc(mm->b, mm->e += mm->buf);
				if (!mm->b) return ERRMEM;

				mm->g = mm->buf;
			}
			else
				mm->g = mm->e - mm->i;

			mm->g = cb(f, mm->b + mm->i, mm->g);
			if (!mm->g)
			{
				*value = mm->b + start;
				*taglen = mm->i - start - *valuelen - 1;
				*tag = (mm->b + start + mm->i + mm->g) - *taglen;
				return ERREND;
			}

		}
	}
	mm->g -= ((size_t)(s - mm->b) + 1) - mm->i;
	mm->i = (size_t)(s - mm->b) + 1;

	*value = mm->b + start;
	*taglen = (size_t)(s - (mm->b + start + *valuelen) - 1);
	*tag = s - *taglen;

	return XML_OK;
}

static int nextpair_insitu(const unsigned char **value, size_t *valuelen, const unsigned char **tag, size_t *taglen, Insitu *mm)
{
	const unsigned char *s;

	if (mm->i >= mm->e || !(s = memchr(mm->b + mm->i, '<', mm->e - mm->i)))
	{
		*value = mm->b + mm->i;
		*valuelen = mm->e - mm->i;
		*taglen = 0;
		*tag = (unsigned char*)"";
		return ERREND;
	}
	*value = mm->b + mm->i;
	*valuelen = (size_t)(s - *value);
	mm->i += *valuelen + 1;

	if (mm->i >= mm->e || !(s = (mm->b[mm->i] == '!' ? memchrignore(mm->b + mm->i, mm->e - mm->i) : memchr(mm->b + mm->i, '>', mm->e - mm->i))))
	{
		*taglen = mm->e - mm->i;
		*tag = mm->b + mm->i;
		return ERREND;
	}
	*tag = mm->b + mm->i;
	*taglen = (size_t)(s - *tag);
	mm->i += *taglen + 1;

	return XML_OK;
}

int worker_nop(int typ, const unsigned char *tag, size_t taglen, const Parser *p)
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
	return XML_OK;
}

int worker_csv(int typ, const unsigned char *tag, size_t taglen, int out(), void* f, Parser *p)
{
	switch (typ)
	{
	case NORMALCLOSE_:
		if (!out(f, "\"", 1)) return ERROUTPUT;
		p->stat.obytes += 1;
		if (!out(f, p->content, p->contentlen)) return ERROUTPUT;
		p->stat.obytes += p->contentlen;
		if (!out(f, "\";", 2)) return ERROUTPUT;
		p->stat.obytes += 2;
		p->userdata = (void*)1;
		break;
	case OPENTAG_:
		break;
	case SELFCLOSE_:
		if (!out(f, "\"\";", 3)) return ERROUTPUT;
		p->stat.obytes += 3;
		p->userdata = (void*)1;
		break;
	case FRAMECLOSE_:
		if (p->userdata)
		{
			if (!out(f, "\n", 1)) return ERROUTPUT;
			p->stat.obytes += 1;
			p->userdata = 0;
		}
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

int worker_clean(int typ, const unsigned char *tag, size_t taglen, int outcb(), void* outcbdata, Parser *p)
{
	switch (typ)
	{
	case NORMALCLOSE_:
		if (!outcb(outcbdata, p->content, p->contentlen)) return ERROUTPUT;
		p->stat.obytes += p->contentlen;
	case OPENTAG_:
	case SELFCLOSE_:
	case FRAMECLOSE_:
		/*case COMMENT_:*/
		/*case UNKNOWN_:*/
	case PROLOG_:
		if (!outcb(outcbdata, tag - 1, taglen + 2)) return ERROUTPUT;
		p->stat.obytes += taglen + 2;
	}
	return XML_OK;
}

int worker_pretty(int typ, const unsigned char *tag, size_t taglen, int outcb(), void* outcbdata, Parser *p)
{
	switch (typ)
	{
	case NORMALCLOSE_:
		if (!outcb(outcbdata, p->content, p->contentlen)) return ERROUTPUT;
		p->stat.obytes += p->contentlen;
		if (!outcb(outcbdata, tag - 1, taglen + 2)) return ERROUTPUT;
		p->stat.obytes += taglen + 2;
		if (!outcb(outcbdata, "\n", 1)) return ERROUTPUT;
		p->stat.obytes += 1;
		p->newline = 1;
		break;
	case OPENTAG_:
	{
		int i = p->ebene;
		if (!p->newline&&i)
		{
			if (!outcb(outcbdata, "\n", 1)) return ERROUTPUT;
			p->stat.obytes += 1;
		}
		while (i--)
		{
			if (!outcb(outcbdata, "\t", 1)) return ERROUTPUT;
			p->stat.obytes += 1;
		}
		if (!outcb(outcbdata, tag - 1, taglen + 2)) return ERROUTPUT;
		p->stat.obytes += taglen + 2;
		p->newline = 0;
		break;
	}
	case FRAMECLOSE_:
	case SELFCLOSE_:
	case COMMENT_:
	case PROLOG_:
	{
		int i = p->ebene;
		if (!p->newline&&i)
		{
			if (!outcb(outcbdata, "\n", 1)) return ERROUTPUT;
			p->stat.obytes += 1;
		}
		while (i--)
		{
			if (!outcb(outcbdata, "\t", 1)) return ERROUTPUT;
			p->stat.obytes += 1;
		}
		if (!outcb(outcbdata, tag - 1, taglen + 2)) return ERROUTPUT;
		p->stat.obytes += taglen + 2;
		if (!outcb(outcbdata, "\n", 1)) return ERROUTPUT;
		p->stat.obytes += 1;
		p->newline = 1;
	}
	}
	return XML_OK;
}

int parse(Parser *p)
{
	/* setting defaults */
	size_t(*inputcb)() = p->inputcb ? p->inputcb : readblock;
	size_t(*outputcb)() = p->outputcb ? p->outputcb : writeblock;
	void   *inputcbdata = p->inputcbdata ? p->inputcbdata : stdin;
	void  *outputcbdata = p->outputcbdata ? p->outputcbdata : stdout;
	int(*worker)() = p->worker ? p->worker : worker_clean;

	const unsigned char *content = 0, *tag = 0;
	size_t contentlen = 0, taglen = 0;
	int myebene = p->ebene, r, neu = 1;

	p->mm.buf = p->buflen ? p->buflen : 65536U;

	while ((r = nextpair(&content, &contentlen, &tag, &taglen, &p->mm, inputcb, inputcbdata)) == XML_OK)
	{
		{ /* nur Statistik */
			(!p->stat.tag_min) || p->stat.tag_min > taglen ? p->stat.tag_min = taglen : 0;
			(!p->stat.content_min) || p->stat.content_min > contentlen ? p->stat.content_min = contentlen : 0;
			p->stat.tag_max < taglen ? p->stat.tag_max = taglen : 0;
			p->stat.content_max < contentlen ? p->stat.content_max = contentlen : 0;
			p->stat.ibytes += (unsigned long)(2 + taglen + contentlen);
		}

		p->content = content;
		p->contentlen = contentlen;
		if (*tag == '/')
		{
			/* NORMALCLOSE_:  Schließtag direkt nach Öffnen-Tag (Normaltag) */
			/* FRAMECLOSE_:  zw. Schließtag und zugehörigem Öffnentag liegen noch andere Tags (Frametag) */
			p->ebene--;

			if ((r = worker(neu ? NORMALCLOSE_ : (p->stat.tag--, p->stat.frametag++, FRAMECLOSE_), tag, taglen, outputcb, outputcbdata, p)) != XML_OK) return r;

			return XML_OK; /* und natürlich wieder eine Ebene zurück */
		}
		else
			if (*tag == '!')
			{
				/* Kommentar */
				p->stat.comment++;
				if ((r = worker(COMMENT_, tag, taglen, outputcb, outputcbdata, p)) != XML_OK) return r;
			}
			else
				if (*tag == '?')
				{
					/* Prolog */
					p->stat.prolog++;
					if ((r = worker(PROLOG_, tag, taglen, outputcb, outputcbdata, p)) != XML_OK) return r;
				}
				else
					if (tag[taglen - 1] == '/')
					{
						/* self closing tag */
						p->stat.selfclose++;
						countAttributes(tag, taglen, &p->stat.attributes);
						if ((r = worker(SELFCLOSE_, tag, taglen, outputcb, outputcbdata, p)) != XML_OK) return r;
					}
					else
					{
						/* Abstieg - Rekursion */
						if ((r = worker(OPENTAG_, tag, taglen, outputcb, outputcbdata, p)) != XML_OK) return r;
						if ((r = addPath(tag, taglen, p->ebene, &p->path)) != XML_OK) return r;
						countAttributes(tag, taglen, &p->stat.attributes);
						p->ebene++;
						p->stat.ebenemax < p->ebene ? p->stat.ebenemax = p->ebene : 0;
						p->stat.tag++;
						if ((r = parse(p)) != XML_OK)
							return r;
					}

		neu = 0;
	}

	if (myebene != p->ebene)
		return ERRHIERAR;

	if (contentlen | taglen)
	{
		if ((r = worker(UNKNOWN_, tag, taglen, outputcb, outputcbdata, p)) != XML_OK)
			return r;
	}
	p->stat.ibytes += (unsigned long)(contentlen + (taglen ? taglen + 1 : 0));

	return XML_OK;
}

int parse_insitu(Parser *p)
{
	/* setting defaults */
#if 0
	size_t(*outputcb)() = p->outputcb ? p->outputcb : writeblock;
	void  *outputcbdata = p->outputcbdata ? p->outputcbdata : stdout;
	int(*worker)() = p->worker ? p->worker : worker_clean;
#endif
	const unsigned char *content, *tag;
	size_t contentlen, taglen;
	int myebene = p->ebene, r, neu = 1;


	while ((r = nextpair_insitu(&content, &contentlen, &tag, &taglen, &p->insitu)) == XML_OK)
	{
		{ /* nur Statistik */
			(!p->stat.tag_min) || p->stat.tag_min > taglen ? p->stat.tag_min = taglen : 0;
			(!p->stat.content_min) || p->stat.content_min > contentlen ? p->stat.content_min = contentlen : 0;
			p->stat.tag_max < taglen ? p->stat.tag_max = taglen : 0;
			p->stat.content_max < contentlen ? p->stat.content_max = contentlen : 0;
			p->stat.ibytes += (unsigned long)(2 + taglen + contentlen);
		}

		p->content = content;
		p->contentlen = contentlen;
		if (*tag == '/')
		{
			/* NORMALCLOSE_:  Schließtag direkt nach Öffnen-Tag (Normaltag) */
			/* FRAMECLOSE_:  zw. Schließtag und zugehörigem Öffnentag liegen noch andere Tags (Frametag) */
			p->ebene--;

			if ((r = p->worker(neu ? NORMALCLOSE_ : (p->stat.tag--, p->stat.frametag++, FRAMECLOSE_), tag, taglen, p->outputcb, p->outputcbdata, p)) != XML_OK) return r;

			return XML_OK; /* und natürlich wieder eine Ebene zurück */
		}
		else
			if (*tag == '!')
			{
				/* Kommentar */
				p->stat.comment++;
				if ((r = p->worker(COMMENT_, tag, taglen, p->outputcb, p->outputcbdata, p)) != XML_OK) return r;
			}
			else
				if (*tag == '?')
				{
					/* Prolog */
					p->stat.prolog++;
					if ((r = p->worker(PROLOG_, tag, taglen, p->outputcb, p->outputcbdata, p)) != XML_OK) return r;
				}
				else
					if (tag[taglen - 1] == '/')
					{
						/* self closing tag */
						p->stat.selfclose++;
						countAttributes(tag, taglen, &p->stat.attributes);
						if ((r = p->worker(SELFCLOSE_, tag, taglen, p->outputcb, p->outputcbdata, p)) != XML_OK) return r;
					}
					else
					{
						/* Abstieg - Rekursion */
						if ((r = p->worker(OPENTAG_, tag, taglen, p->outputcb, p->outputcbdata, p)) != XML_OK) return r;
						if ((r = addPath(tag, taglen, p->ebene, &p->path)) != XML_OK) return r;
						countAttributes(tag, taglen, &p->stat.attributes);
						p->ebene++;
						p->stat.ebenemax < p->ebene ? p->stat.ebenemax = p->ebene : 0;
						p->stat.tag++;
						if ((r = parse_insitu(p)) != XML_OK)
							return r;
					}

		neu = 0;
	}

	if (myebene != p->ebene)
		return ERRHIERAR;

	if (contentlen | taglen)
	{
		if ((r = p->worker(UNKNOWN_, tag, taglen, p->outputcb, p->outputcbdata, p)) != XML_OK)
			return r;
	}
	p->stat.ibytes += (unsigned long)(contentlen + (taglen ? taglen + 1 : 0));

	return XML_OK;
}

int parse_light_insitu(Parser *p)
{
	const unsigned char *content, *tag;
	size_t contentlen, taglen;
	int myebene = p->ebene, r, neu = 1;

	while ((r = nextpair_insitu(&content, &contentlen, &tag, &taglen, &p->insitu)) == XML_OK)
	{
		p->content = content;
		p->contentlen = contentlen;
		if (*tag == '/')
		{
			/* NORMALCLOSE_:  Schließtag direkt nach Öffnen-Tag (Normaltag) */
			/* FRAMECLOSE_:  zw. Schließtag und zugehörigem Öffnentag liegen noch andere Tags (Frametag) */
			p->ebene--;

			if ((r = p->worker(neu ? NORMALCLOSE_ : FRAMECLOSE_, tag, taglen, p)) != XML_OK) return r;

			return XML_OK; /* und natürlich wieder eine Ebene zurück */
		}
		else
			if (*tag == '!')
			{
				/* Kommentar */
				if ((r = p->worker(COMMENT_, tag, taglen, p)) != XML_OK) return r;
			}
			else
				if (*tag == '?')
				{
					/* Prolog */
					if ((r = p->worker(PROLOG_, tag, taglen, p)) != XML_OK) return r;
				}
				else
					if (tag[taglen - 1] == '/')
					{
						/* self closing tag */
						if ((r = p->worker(SELFCLOSE_, tag, taglen, p)) != XML_OK) return r;
					}
					else
					{
						/* Abstieg - Rekursion */
						if ((r = p->worker(OPENTAG_, tag, taglen, p)) != XML_OK) return r;
						/*if ((r = addPath(tag, taglen, p->ebene, &p->path)) != XML_OK) return r;*/
						p->ebene++;
						if ((r = parse_light_insitu(p)) != XML_OK)
							return r;
					}

		neu = 0;
	}

	if (myebene != p->ebene)
		return ERRHIERAR;

	if (contentlen | taglen)
	{
		if ((r = p->worker(UNKNOWN_, tag, taglen, p)) != XML_OK)
			return r;
	}

	return XML_OK;
}

int parse_light(Parser *p)
{	/* dasselbe wie bei parse nur ohne Statistik und ohne Path */

	const unsigned char *content, *tag;
	size_t contentlen, taglen;
	int myebene = p->ebene, r, neu = 1;

	while ((r = nextpair(&content, &contentlen, &tag, &taglen, &p->mm, p->inputcb, p->inputcbdata)) == XML_OK)
	{
		p->content = content;
		p->contentlen = contentlen;
		if (*tag == '/')
		{
			/* NORMALCLOSE_:  Schließtag direkt nach Öffnen-Tag (Normaltag) */
			/* FRAMECLOSE_:  zw. Schließtag und zugehörigem Öffnentag liegen noch andere Tags (Frametag) */
			p->ebene--;

			if ((r = p->worker(neu ? NORMALCLOSE_ : FRAMECLOSE_, tag, taglen, p)) != XML_OK) return r;

			return XML_OK; /* und natürlich wieder eine Ebene zurück */
		}
		else
			if (*tag == '!')
			{
				/* Kommentar */
				if ((r = p->worker(COMMENT_, tag, taglen, p)) != XML_OK) return r;
			}
			else
				if (*tag == '?')
				{
					/* Prolog */
					if ((r = p->worker(PROLOG_, tag, taglen, p)) != XML_OK) return r;
				}
				else
					if (tag[taglen - 1] == '/')
					{
						/* self closing tag */
						if ((r = p->worker(SELFCLOSE_, tag, taglen, p)) != XML_OK) return r;
					}
					else
					{
						/* Abstieg - Rekursion */
						if ((r = p->worker(OPENTAG_, tag, taglen, p)) != XML_OK) return r;
						p->ebene++;
						if ((r = parse_light(p)) != XML_OK)
							return r;
					}

		neu = 0;
	}

	if (myebene != p->ebene)
		return ERRHIERAR;

	if (contentlen | taglen)
	{
		if ((r = p->worker(UNKNOWN_, tag, taglen, p)) != XML_OK) return r;
	}

	return XML_OK;
}

void done(Parser *p)
{ /* Speicher-free und NULL/0-Setzung */
	while (p->path.bz--) free(p->path.List[p->path.bz].b);
	free(p->path.List);
	free(p->mm.b);
	memset(p, 0, sizeof*p);
}

void init_light(Parser *p,
	void* inputcbdata,
	void* outputcbdata,
	int(*worker)(),
	size_t(*inputcb)(),
	size_t(*outputcb)(),
	void* userdata,
	size_t buflen,
	const unsigned char *b,
	size_t e)
{
	p->inputcbdata = inputcbdata ? inputcbdata : stdin;
	p->outputcbdata = outputcbdata ? outputcbdata : stdout;
	p->worker = worker ? worker : worker_clean;
	p->inputcb = inputcb ? inputcb : readblock;
	p->outputcb = outputcb ? outputcb : writeblock;
	p->userdata = userdata ? userdata : 0;
	p->mm.buf = buflen ? buflen : 65536U;
	p->insitu.b = b ? b : 0;
	p->insitu.e = e ? e : 0;
}
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

#define __USE_MINGW_ANSI_STDIO 1
#define _FILE_OFFSET_BITS  64  /* gcc,clang,mingw support for Files > 4GB on 32Bit also */

#include <stddef.h> /* size_t */

typedef struct
{
	size_t z;
	const unsigned char *b;
} Memblock; /* for match with void *userdata */

typedef struct
{
	unsigned char *b;
	size_t i, g, e, buf;
} MemMan;   /* Memory-Manager für Lesepuffer */

typedef struct
{
	const unsigned char *b; /* points to begin of Memoryblock */
	size_t e;               /* Byte-Size of Memoryblock */
	size_t i;  /* internal! */
} Insitu; /* Memoryblock of full loaded XML document */

typedef struct
{
	size_t
		ebenemax,
		tag,
		frametag,
		selfclose,
		comment,
		prolog,
		content_min,
		content_max,
		tag_min,
		tag_max,
		ibytes,
		obytes;
} Statistik;

typedef struct {
	size_t z;
	unsigned char path[8192];
	size_t taglen;
	unsigned char tag[1024];
	const unsigned char *tagptr;
} Path;

typedef struct
{
	/******************** user-defined ********************/
	void* inputcbdata;
	void* outputcbdata;
	int(*worker)();
	size_t(*inputcb)();
	int(*outputcb)();
	void* userdata;
	Insitu insitu;
	/******************************************************/

		/* internal */
	MemMan mm;
	size_t ebene;
	const unsigned char *content;
	size_t contentlen;
	int newline;
	Path path;
	Statistik stat;
} Parser;

enum { XML_OK, ERRMEM, ERROUTPUT, ERRHIERAR, ERREND };
enum { SELFCLOSE_, COMMENT_, PROLOG_, NORMALCLOSE_, FRAMECLOSE_, OPENTAG_, UNKNOWN_ };

/* Hauptroutine, siehe Parser */
int parse(Parser*);
int parse_insitu(Parser*);
int parse_light_insitu(Parser*);
int parse_light(Parser*);
void init_light(Parser*, void*, void*, int(*)(), size_t(*)(), int(*)(), void*, size_t, const unsigned char*, size_t);

/* Reset Parser (free) */
void done(Parser *);

/* predefined worker-callbacks */
int worker_clean(int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);
int worker_csv(int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);
int worker_nop(int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);
int worker_pretty(int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);
int worker_xpath(int typ, const unsigned char *tag, size_t taglen, int out(), void* f, Parser *p);
int worker_xpath_match(int typ, const unsigned char *tag, size_t taglen, int out(), void* f, Parser *p);
int writeln(void *f, const unsigned char*s, size_t w);

Memblock getattribut(const unsigned char *s, size_t z, Memblock);
int anymatch(const unsigned char *w, size_t z, const unsigned char *s, size_t b);
size_t wordlen(const unsigned char*s, size_t z);
size_t nslen(const unsigned char*s, size_t z);



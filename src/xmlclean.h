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

#include <stddef.h> /* size_t */

typedef struct
{
	unsigned char *b;
	size_t i, g, e, buf;
} MMan;   /* Memory-Manager für Lesepuffer */

typedef struct
{
	const unsigned char *b;
	size_t e;
	size_t i;
} Insitu;

typedef struct
{
	size_t
		ebenemax,
		tag,
		frametag,
		selfclose,
		comment,
		prolog,
		attributes,
		content_min,
		content_max,
		tag_min,
		tag_max,
		ibytes,
		obytes;
} Statistik;

typedef struct {
	size_t bz;
	struct {
		unsigned char *b;
		size_t z;
	} *List;
} Path; /* Opentag-Ebenen */

typedef struct
{
	/******************** user-defined ********************/
	void* inputcbdata;
	void* outputcbdata;
	int(*worker)();
	size_t(*inputcb)();
	size_t(*outputcb)();
	void* userdata;
	size_t buflen;
	Insitu insitu;
	/******************************************************/

		/* internal */
	MMan mm;
	unsigned long ebene;
	const unsigned char *content;
	size_t contentlen;
	int newline;
	Path path;
	Statistik stat;
} Parser;

enum { XML_OK, ERRMEM, ERROUTPUT, ERRHIERAR, ERREND };
enum { SELFCLOSE_, COMMENT_, PROLOG_, NORMALCLOSE_, FRAMECLOSE_, OPENTAG_, UNKNOWN_ };

/* Hauptroutine, siehe Parser */
int parse(Parser *);
int parse_insitu(Parser *);
int parse_light_insitu(Parser *);
int parse_light(Parser *);
void init_light(Parser*, void*, void*, int(*)(), size_t(*)(), size_t(*)(), void*, size_t, const unsigned char*, size_t);

/* Reset Parser (free) */
void done(Parser *);

/* predefined worker-callbacks */
int worker_clean(int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);
int worker_csv(int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);
int worker_nop(int typ, const unsigned char *tag, size_t taglen, const Parser*);
int worker_pretty(int typ, const unsigned char *tag, size_t taglen, int(), void*, Parser*);


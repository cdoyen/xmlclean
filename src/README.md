### Allgemeines

Der Code arbeitet objektorientiert und mit Rekursion.  
Als interne Datenstruktur dient ein doppelt gepufferter Memoryblock-Stream.  

### API allgemein

Der Nutzer muss ein zentrales Objekt vom Typ Parser (struct) konfigurieren. Dies geschieht ausschließlich über Callback-Funktionen jeweils mit userdata.  
Der Worker-Callback wird für jedes Paar aus Tag + vorigem Content aufgerufen, z.B. für
```xml
<foo><bar>baz</bar></foo>
```
werden worker-Callback-Aufrufe mit folgenden Daten erzeugt:
```
"" + "foo"
"" + "bar"
"baz" + "/bar"
"" + "/foo"
```
Diese Daten sind dabei keine Strings sondern Bytefolgen mit Längenangabe.

### API konkret

xmlclean.h:  
```c
typedef struct
{
/******************** user-defined ********************/
	void* inputcbdata;
	void* outputcbdata;
	int (*worker)();
	size_t(*inputcb)();
	size_t(*outputcb)();
	void* userdata;
	size_t buflen;
/******************************************************/
```
Für alle Elemente sind Defaults vorhanden, die jeweils bei NULL/0 Initialisierung im Parser-Objekt verwendet werden.

* inputcbdata: Zeiger auf userdata des Input-Callbacks (Default: stdin)
* outputcbdata: Zeiger auf userdata des Output-Callbacks (Default: stdout)
* worker: Callback zur Verarbeitung der o.g. Daten-Paare (Default: worker_clean)
* inputcb: Callback zum Lesen des Inputstreams (Default: Lesen aus FILE*)
* outputcb: Callback zum Schreiben des Outputstreams (Default: Schreiben in FILE*)
* userdata: userdata für worker-Callback (Default: NULL)
* buflen: Puffergröße für Inputstream (Default: 65536)

### worker-Callback
```c
int worker_nop(int typ, const unsigned char *tag, size_t taglen, int outcb(), void* outcbdata, Parser *p)
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
```
Neben dem XML-konform extrahierten Datenpaar Content+Tag wird auch noch die output-Callback, das Parserobjekt selber sowie der Typ des Datenpaares übergeben. Auf den Content wird *indirekt* über das Parser-Objekt zugegriffen:  
`p->content   (const unsigned char*)`  
`p->contentlen (size_t)`  

Ausgehend vom Beispiel
```xml
<?xml version="1.0"?>
<!--Introduction-->
<foo>
<bar>baz</bar>
<baz foo/>
</foo>Müll<Schrott
```
ergeben sich dann die Typen und Wertepaare wie folgt:
```c
NORMALCLOSE_ : "baz" + "/bar"
OPENTAG_     : "\n" + "foo" , "\n" + "bar"
SELFCLOSE_   : "\n"  +"baz foo/"
FRAMECLOSE_  : "\n" + "/foo"
COMMENT_     : "\n"  +"!--Introduction--"
PROLOG_      : "" + "?xml version="1.0"?"
UNKNOWN_     : "Müll " + "Schrott"
```
Der Returnwert ergibt sich wie folgt:
```c
XML_OK   : fehlerfrei
ERRMEM   : Fehler bei Hauptspeicher-Anforderung
ERROUPUT : Fehler beim Schreiben auf output-Callback
ERRHIERAR: XML-Baum unvollständig
```
### Input-Callback
```c
size_t inputcb(void *inputcbdata, unsigned char*, size_t)
{
   ...
}
```
Diese Callback liest die Anzahl Bytes aus *inputcbdata* (z.B. FILE*) und gibt die Anzahl der tatsächlich gelesenen Bytes zurück.  
0 bedeutet dabei, dass das Ende des Inputstreams erreicht ist.
### Output-Callback
```c
size_t outputcb(void *outputcbdata, const unsigned char*, size_t)
{
   ...
}
```
Diese Callback schreibt die Anzahl Bytes in *outputcbdata* (z.B. FILE*) und gibt 0 zurück, falls nicht alle Bytes geschrieben werden konnten, sonst einen Wert ungleich 0.
### Statistik
**xmlclean** enthält einige statistische Auswertungen über den geparsten Datenbestand, siehe hierzu u.a. Beispiel *nop*. Das Beispiel sollte selbsterklärend sein. 
```c
typedef struct
{
	unsigned long
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

typedef struct
{
...
	unsigned long ebene;  /* aktuelle Ebene im XML-Baum */
	const unsigned char *content; /* Content-Bytes für aktuellen Tag */
	size_t contentlen;  /* Länge Content-Bytes */
	int newline;    /* dummy-Flag ähnlich userdata */
	Path path;      /* Opentag-Hierarchie */
	Statistik stat; /* Statistik */
} Parser;
```
Die Statistik-Angabe  
`->stat.obytes`  
muss der Anwender im Output-Callback selbst pflegen (Aufsummieren der über Output-Callback ausgegebenen Bytes).
Zusätzlich zum Einstieg mit *parse()* bietet **xmlclean** *parse_light()* an. Hierbei wird gegenüber *parse()* keine Statistik gepflegt.  
Zum Verständnis der rekursiven Arbeitsweise von **xmlclean** ist *parse_light()* aufgrund des fehlenden Statistik-Codes besser geeignet.

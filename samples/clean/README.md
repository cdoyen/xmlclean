### Beispiel für worker-callback "worker_clean"

```c
	Parser p = { 0 };
	parse(&p);
	done(&p);
```
Zunächst wird ein Parser-Objekt definiert und sämtlichst mit 0/NULL initialisiert.
Das führt dazu, dass der Parservorgang ausschließlich mit den in **xmlclean** festgelegten Default-Werten gestartet wird, insbesondere sind dies  
* input-callback arbeitet auf `FILE*` und `stdin`
* output-callback arbeitet auf `FILE*` und `stdout`
* worker-callback ist auf `worker_clean`gesetzt

Der Aufruf
```bash
./clean < countries.xml > countries.clean.xml
```
führt dann zum gezeigten Ergebnis und einer XML-Datei mit reduziertem aber gleichem Effektiv-Inhalt;  
es werden alle Kommentare und alle "Formatierungs"-Zeichen zwischen den Tags (Space,Newline,...) entfernt.
```c
void worker_clean(int typ, const unsigned char *tag, size_t taglen, int outcb(), void* outcbdata, Parser *p)
{
	switch (typ)
	{
	case NORMALCLOSE_:
		outcb(outcbdata, p->content, p->contentlen);
	case OPENTAG_:
	case SELFCLOSE_:
	case FRAMECLOSE_:
	/*case COMMENT_:*/
	/*case UNKNOWN_:*/
	case PROLOG_:
		outcb(outcbdata, tag - 1, taglen + 2);
	}
}
```
Die Callback-Funktion zeigt, dass Kommentare und ungültige endende XML-Dateiinhalte nicht zur Ausgabe über den output-Callback gelangen.  
Für normale (Schließ)Tags wird zusätzlich zuerst ihr Content ausgegeben, anschließend ihr eigentlicher Taginhalt, ebenso wie bei allen übrigen Tagtypen.  

Das einfache Beispiel:
```xml
<!-- blah -->
<foo id="1">
	<bar>baz</bar>
	<baz>bar</baz>
</foo>
```
wird also beispielsweise zu:
```xml
<foo id="1"><bar>baz</bar><baz>bar</baz></foo>
```
was der ursprünglichen Intention von **xmlclean** entspricht.


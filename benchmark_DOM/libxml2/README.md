Libxml2 ist eine äußerst schlecht implementierte Bibliothek.  

Von Zitat:  
`is known to be very portable, the library should build and work without serious troubles on a variety of systems`  
kann **keine Rede** sein.  

Der Code ist vollständig durchsetzt von Plattform- und Compilerabhängigkeiten.  
Code wie  
```C
	if (node->line == 65535) {
	    if ((node->type == XML_TEXT_NODE) && (node->psvi != NULL))
	        result = (long) node->psvi;   /* psvi ist hier ein Pointer */
```  
oder
```C
ret = read((int) (long) context, &buffer[0], len);
...
ret->context = (void *) (long) fd;
...
ret->psvi = (void *) (long) ctxt->input->line;
```
trägt ganz sicher nicht zu korrekten Ergebnissen und schon gar nicht zu portablem Code bei.  
Folgerichtig crasht Libxml2 auch bei Daten, die nicht im Fokus des Autors standen (> 2GB), siehe `crash.jpg`.  

Auch die Performanz lässt sehr zu wünschen übrig, trotz idealisierter Testumgebungen (in Memory + SAX) liegt Libxml2 abgeschlagen am Ende.  
Der Code unübersichtlich und viel zu umfangreich, insbesondere die exorbitant vielen Compilierungsschalter machen den Code für einen ernsthaften Einsatz unbrauchbar.  

Die Quelldateien liegen ungeordnet - Test + Bibliothekscode zusammen in einem Verzeichnis - vor, die Unmenge an Compilierungsschalter sind **nicht** unabhängig voneinander benutzbar.


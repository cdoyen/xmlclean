## xmlclean

### ANSI C Bibliothek für das Parsen von XML

* strikt konform gemäß ANSI C/C89
* reentrant/multithreadfähig
* SAX basiert (kein DOM/XPath)
* keine Interpretierung von Daten-Inhalten
* keine Prüfung auf XML-Konformität
* keine XSD-Validierung
* keine Encoding-Behandlung

## Intention

In Zeiten von Bigdata wird XML unvernünftigerweise genau dafür genutzt, oftmals muss man mit großen XML-Dateien hantieren, deren effektive Daten (abzüglich Tags, Kommentaren, Namespaces,...) deutlich geringer ausfallen würden.  
  
Hier setzt **xmlclean** an, der Parser bietet eine einfache Möglichkeit, auf rohe XML-Daten zuzugreifen.  
Die Rohdaten werden 1:1 (d.h. ohne jegliche Prüfung/Konvertierung,...) gemäß XML-Regeln aus dem XML-Stream extrahiert und an die Callback-Funktion übergeben.  **xmlclean** entspricht somit einem klassischen Parser.  
**xmlclean** bietet eine vordefinierte Callback-Funktion an, um redundanten Daten aus XML-Dateien zu entfernen. (siehe Beispiel ***clean***)    
Die Bibliothek ist sehr klein, sehr schnell, sehr portabel und benötigt wenig Hauptspeicher.

## Lizenz

LGPL 2.1

## xmlclean

### ANSI C Bibliothek für das Parsen von XML

* strikt konform gemäß ANSI C/C89
* reentrant/multithreadfähig
* SAX basiert, aber auch in situ möglich
* keine Interpretierung von Daten-Inhalten
* keine Prüfung auf XML-Konformität
* keine XSD-Validierung
* keine Encoding-Behandlung

#### Benefit zu anderen SAX-Parsern
* Typinfo des Tags (Normal-,Frame-,self closing,Kommentar...) ist bei jedem Callback verfügbar
* XPATH Zugriff ist bei jedem SAX-Callback möglich

## Intention

In Zeiten von Bigdata wird XML unvernünftigerweise genau dafür genutzt, oftmals muss man mit großen XML-Dateien hantieren, deren effektive Daten (abzüglich Tags, Kommentaren, Namespaces,...) deutlich geringer ausfallen würden.  
  
Hier setzt **xmlclean** an, der Parser bietet eine einfache Möglichkeit, auf rohe XML-Daten zuzugreifen.  
Die Rohdaten werden 1:1 (d.h. ohne jegliche Prüfung/Konvertierung,...) gemäß XML-Regeln aus dem XML-Stream extrahiert und an die Callback-Funktion übergeben.  **xmlclean** entspricht somit einem klassischen Parser.  
**xmlclean** bietet eine vordefinierte Callback-Funktion an, um redundanten Daten aus XML-Dateien zu entfernen. (siehe Beispiel ***clean***)    
Die Bibliothek ist sehr klein, sehr schnell, sehr portabel und benötigt wenig Hauptspeicher.

## Lizenz

LGPL 2.1

Das Beispiel zeigt, wie die Suche nach Tags in der Callback-Funktion abläuft, in diesem Beispiel wird innerhalb der XML-Datei *countries.xml* gleich nach mehreren Tags gesucht und bei Erfolg der Inhalt (content) ausgegeben.  
* in main wird das Such-Pattern "name|\*capital\*|\*tude" vorgegeben
* in der Callback-Funktion wird mit der Library-Funktion *anymatch* bei (Close)-Tags nach Übereinstimmung gesucht
* da hierbei nicht auf den XPATH zugegriffen wird sondern nur auf die Tags, kann die schnellere Variante *parse_light* statt *parse* verwendet werden  

Zu beachten ist hierbei:  
* *anymatch* vergleicht immer ignore-case
* *anymatch* akzeptiert die üblichen Stringmatching-Zeichen '\*' und '?'
* an *anymatch* dürfen *keine* Großbuchstaben übergeben werden

Beispielaufruf:
```bash
./worker_search countries.xml >countries_search.txt
```

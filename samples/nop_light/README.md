Das Beispiel zeigt die Verwendung des vordefinierten worker-Callbacks *worker_nop*, diesmal aber mit parse_light() statt parse().
Der Aufruf
```bash
./nop countries.xml
```
parst die XML-Datei und gibt keine Statistik dafür aus, da parse_light() im Gegensatz zu parse() *keine* Statistik pflegt.

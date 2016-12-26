Expat ist eine verbreitete C-Library vorwiegend für das Parsen und anschließende native Verarbeitungen von XML Strings auf SAX Basis.  
Von ["which is well-known being the fastest XML parser on earth"](https://web.archive.org/web/20140908021713/http://chneukirchen.org/blog/archive/2004/12/push-vs-pull.html) kann jedoch **KEINE** Rede sein.  

Der Quellcode ist für die einfache Aufgabe von XML-Parsen viel zu umfangreich (400KB), es werden unnötige Compiler- und Plattformabhängigkeiten geschaffen, die zwangsläufig zu Unübersichtlichkeit, Unportabilität, Inperformanz führen.  
Das zeigt schon der einfache Versuch: trotz 64-Bit Compilat scheitert Expat an XML-Dateigrößen >1,4 GByte.  
Code wie
```C
            charDataHandler(handlerArg,
                            (XML_Char *)s,
                            (int)((XML_Char *)next - (XML_Char *)s));
```
ist sinnfrei (expliziter Pointercast verhindert Compilerwarnungen, Pointersubtraktion mit sinnfreiem (int) Cast statt korrekter Verwendung von `ptrdiff_t`).  
Der C-Standard wird ignoriert; statt den Standard konsequent anzuwenden werden laienhaft Annahmen getroffen:  
```C
/* Handle the case where memmove() doesn't exist. */
#ifndef HAVE_MEMMOVE
```
während an anderen Stellen `memcpy` verwendet wird: `memmove` und `memcpy` sind beide schon seit C89 Bestandteil des Standards.  
Die Aufzählungen an sinnfreien und falschen Codestellen ließe sich ohne Probleme weit fortführen:  
#### Schulnote 4#####


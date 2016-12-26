Oracle XDK (**X**ML **D**evelopment **K**it) for C ist keine OpenSource-Software.  
Als Teil der Oracle-DB wird es lediglich als lib/a/dll/so mit Headerdateien geliefert.  
Für Windows existiert diese XDK nur als MSVC und 32Bit Variante.  
Für MinGW oder 64Bit ist demzufolge unter Windows kein Einsatz möglich.  

Die API liegt für C, C++ und Java vor, ist sehr umfangreich, und geht weit über das bloße XML-Parsen (was hier ausschließlich Gegenstand der Betrachtung ist) hinaus.  
Die Performanz ist sehr schlecht, vom vom *Senior Development Manager* im *Developing XML Applications with Oracle XML DB and Oracle
XML Developer’s Kit Guide* angepriesenen **High performance XML Parser** kann **KEINE** Rede sein.  

Zusätzlich zum o.g. eingeschränkten Einsatzbereich ist auch das Sourcecode-Handling schlecht konzipiert:  
* es liegt eine Proxy-Lib vor gegen die man (statisch) linken muss, die selbst aber wiederum auf DLL verweist - das ist irreführend und sinnfrei (es muss oraxml10.lib UND oraxml10.dll vorhanden sein)
* d.h. es liegt für jedes XDK-Compilat immer eine Abhängigkeit von mind. 5 DLL vor
* zusätzlich müssen alle Oracle-NLS Files vorliegen, was kleine schlanke Anwendungen unmöglich macht
* trotz der umfangreichen NLS-Files kann man leicht inhaltslose Fehlermeldungen erhalten  
`Message 9 not found; No message file for product=XDK, facility=LPX
LPX-00200: Message 200 not found; No message file for product=XDK, facility=LPX`
* weiterhin fehlt eine simple Quellcode-Versionierung, das Kit das Oracle-DB 8,9,10,11,12 beiliegt enthält diesbezüglich keine Information

#### Schulnote 5

asm-xml läuft hier außer Konkurrenz, da diese Library zu jedem XML-File zwingend immer noch ein passendes XSD-Schema-File benötigt.  
Zusätzlich zur ohnehin schon ziemlich idealisierten DOM-Voraussetzung, dass das XML-Dokument **vollständig** im Speicher vorliegen muss und dieser Speicher durch den Parser zerstört werden darf, ist diese Voraussetzung in den meisten Praxisfällen überhaupt nicht erfüllbar, zumal das geforderte XSD-Schema offensichtlich Spezialanforderungen von asm-xml erfüllen muss um akzeptiert zu werden - außerhalb der von asm-xml selbst gelieferten XML +XSD-Testdaten war es mir jedenfalls für alle anderen XML-Files nicht möglich, ein asm-tree "konformes" XSD-File zu erstellen (das jeweils aber immer W3C konform(!) war).  
Weiterhin arbeitet asm-xml nur mit 32Bit, also max. mit 4GB (in der Praxis oft nur max. 1,4GB) großen Dateien/Speicherbereichen.  

Schulnote: 5



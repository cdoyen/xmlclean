Die MSXML Bibliothek von Microsoft existiert seit vielen Jahren, was aber kein Qualitätsmerkmal ist. Trotz verschiedener Versionen (aktuell ist 6.0) ist MSXML für das Parsen von XML-Files denkbar ungeeignet, die API völlig überladen, die Performanz sehr schlecht.  
Es werden unnötige Abhängigkeiten geschaffen, COM ist zuwingend erforderlich. Dadurch wird die Bibliothek zwangsläufig unportabel und inperformant.  
Trotz großzügiger Gestaltung des Messprogramms (COM-Initialisierung/Deinitialisierung geht nicht in die Messung ein) liegt MSXML am Ende des Feldes.  

Berücksichtigt man nun noch, dass trotz Verwendung von MS-COM, MS-XML, MS-VisualStudio und MS-Windows große XML-Dateien zu Abstürzen ohne jegliche Fehlermeldungen führen:  
####Schulnote 5####

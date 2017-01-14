VTD ist ein schlecht implementierter Versuch, XML zu verarbeiten und keineswegs die versprochene `The Future of XML Processing`.  
Die Nutzung des Parsergenerators `flex` für einen Teil des Quellcodes und die dadurch vorhandene mächtige Portabilität wird durch eigene Versuche des Autor zunichte gemacht, beispielsweise wird der Standardtyp `long` unnötig in einen eigenen - explizit 32Bit - Typ `Long` gewandelt, was zwangsläufig die Portabilität konterkariert.  
Demzufolge verwundert es nicht, dass VTD an großen XML-Files scheitert.  

Solch eine unnötige laienhafte Aufgabe der Portabilität grenzt an Blasphemie und hat deswegen nur eine Bewertung verdient:
####Schulnote 6

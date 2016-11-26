sxml ist eine kleine übersichtliche C-Bibliothek, SAX und DOM werden unterstützt.  
Leider lässt die Performanz sehr zu wünschen übrig, denn anders als **xmlclean** werden hier Strings und malloc/realloc beim jedem Node/Tag verwendet.  
Bei größeren XML-Files stürzt sxml (trotz 64Bit-Compilat) ab.  

Schulnote: 4

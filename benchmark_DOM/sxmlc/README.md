sxml ist eine kleine übersichtliche C-Bibliothek, SAX und DOM werden unterstützt.  
Leider lässt die Performanz sehr zu wünschen übrig, denn anders als **xmlclean** werden hier Strings und Einzel-malloc/realloc bei jedem Node/Tag verwendet.  
Bei größeren XML-Files stürzt sxml (trotz 64Bit-Compilat) ab.  

Schulnote: 4

# Anzeigetafel

# Anzeigetafel

Aktuell dient diese Readme als Arbeitsjournal, damit einfacher Dokumentiert werden kann.

25.09.25
Kickoff Meeting mit Firouzi

25.09 - 09.10.25
Material bestimmung mit AliExpress und AliBaba Produkten.
Siehe OneNote Dokument.

09.10.25
Bestellungsaufgabe an Martin Löw.(Erwartete Ankunft 07.11.25)

05.11.25
Rpi mit dem Debian aufgesetzt und mit git, ssh und vnc eingerichtet.
Das Repo aus dem Gitlab vom FS25 OOP-Software Programmieren geklont und auf Rpi getestet. Das Program wird auf Rpi mit CMake kompiliert und ausgeführt.
Dazu musste eine CmakeLists.txt Datei erstellt werden, die die Quelldateien und Abhängigkeiten definiert.

06.11.25
Einfachheits halber wurden die Quelldateinen von Gitlab in ein Github repo kopiert, damit das Sofware zu einer späteren Zeitpunkt einfacher angepasst werden kann.
Das program wird als Opensource Programm ausgeführt und benötigt daher keine Lizenzen.
Durch eine build ordner Stuktur kann das Program ohne komplirieren zu müssen ausgeführt werden.
Beim booten den Rpi wird automatisch das Programm gestartet und die Anzeigetafel angezeigt.

07.11.25
Die Software wurde so angepasst, dass beim Programmstart das scorer_board - Fenster direkt in Vollbildmodus startet.
Eine Verknüpfung des Programms wurde auf der Desktop installiert. Die Verknüpfung hat das Vereinssymbol als Icon.

07.11.25 bis 27.11.25
Siehe Github. Softwaremässig wurde eine Zustandmaschine implementiert, die es ermöglicht, zwischen verschiedenen Anzeigemodi zu wechseln.
PreGame, FirstHalf, HalfTime, SecondHalf, PostGame -> Sind die verschiedenen Zustände.

27.11.25
Die vier Matrizen wurden zusammen verkabelt und es wurde versucht eine Testbild anzuzeigen. Leider konnten nichts angezeigt werden.
Es scheint so als wären die Matrizen defekt, denn es leuchten keine LED's wenn Spannung angelegt wird. Dies wäre normal wenn man die Verbindungen bei laufender Spannung am Matrix einsteckt.
Auch beim Einstecken der HUB verbindung ist nichts zu sehen.
Als Backup wurden 4 P5 Matrizen auf AliExpress bestellt. Diese sollten in ca. 2 Wochen ankommen.

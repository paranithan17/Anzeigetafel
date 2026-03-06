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

04.12.25
Die vorhanden Matrizen wurden nun als Prototyp zusammengebaut. Es konnte kein Bild angezeigt werden. Um auszuschliessen, dass die HUIDU Sende und Controller Einheit defekt sind, wurde der Aufbau mit dem P2.5 Matrizen von Lyson (privater Besitz) getestet. Mit diesem Aufbau konnte ein Bild angezeigt werden.

11.12.25
In dieser Zeit wurde mit dem Hersteller der P2.5 Panele Kontakt aufgenommen. Nach dem er mir einige Details nennen konnte, konnte zumindest festgestellt werden, dass die Panele nicht defekt sind. Man konnte vorerst einen RGB-Test durchführen, bei dem die einzelnen Farben Rot, Grün und Blau nacheinander angezeigt werden.

16.12.25
Inziwschen Zeit wurde auch mit der Firma HUIDU Kontakt aufgenommen. Diese haben mir per Remote Zugriff Support für die Konfiguration LED Wands gegeben.
Nun kann ich die P2.5 Panele mit der HUIDU Sendeeinheit verbinden und auch ein Testbild anzeigen.
Aktuell wird die Auflösung der Anzeigeteifel ziemlich stark zerquetssch auf den Panels aussgegeb, so dass das Bild nicht wirklich gut aussieht.
Mit der Software HDPlayer können nebst der HDMI-Quelle diverse andere Vislualisierungen erstellt und diese dann auf die LED Wand gestreamt werden. Diese Viusalisierungen hingegen sind in guter Qualität sichtbar.

22.12.25 - 25.12.25
Die Software wurde geupdatet. Siehe Git Commit History.
https://news.sparkfun.com/2650
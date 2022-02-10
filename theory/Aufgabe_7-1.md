# Aufgabe 7-1  (Integration)  – 5  Punkte

_Auf einem PC läuft eine Anwendung zur Wiedergabe von Musik. Die Musikdateien liegen auf der lokalen Festplatte. Die Wiedergabe wird in unregelmäßigen Momenten unterbrochen, sodass der Musikgenuss sehr eingeschränkt ist. Wie gehen Sie zur Problembehebung vor?                
Begründen Sie kurz die einzelnen Schritte._

0. Prüfen, ob das fragliche Album in Vinyl vorliegt, wenn ja: Medium wechseln. Schallplatten werden grundsätzlich als einzelner sequenzeieller Ausführungsstrang (AKA Rille) mit maximaler Priorität ausgeführt, wodurch alle durch Nebenläufigkeit bedingten Probleme ausgeschlossen werden können.

1. Annahme: Moderene Systeme erlauben ein hohes Maß an Nebenläufigkeit und Parallelität. Die Interaktion diverser Schichten ist dabei häufig komplex verflochten. Zuweilen werden Schichtenmodelle mutwillig ignoriert und zugunsten höherer Perfomanz durchbrochen. Dies kann zu allen Arten von unerwünschtem und kaum noch nachvollziehbarem Verhalten führen. \
Behebung: Systeme befinden sich beim Starten (und in Annährung kurz danach) in wohldefinierten Zuständen. In diesen definierten Startzuständen ist die Chance, dass oben beschriebene Effekte auftreten geringer. Sprich, Rechner ausschalten, wieder einschalten und prüfen, ob das Problem weiter besteht.

2. Annahme: Es gibt zu viele nebenläufig ausgeführte Prozesse, sodass der Musik-Player zu selten an die Reihe kommt, um eine lückenlose Wiedergabe umzusetzen. (Flaschenhals CPU oder Festplatte, falls die Menge an Prozessen zu Thrashing führt) \
Behebung: Andere Porzesse beenden (oder falls möglich, die Priorität des Player-Prozesses erhöhen) und prüfen, ob das Problem weiter besteht.

3. Annahme: Der Musik-Player lädt die Audio-Datein nicht als ganzes, sondern Stückweise in den Hauptpeicher. Es gibt zu viele IO-Anfragen anderer Prozesse an die Festplatte, sodass das Nachladen der Musik nicht schnell genug geht. (Flaschenhals Festplatte) \
Behebung: Prüfen, ob es andere Prozesse gibt, die mit der Festplatte interagieren; diese ggf. beenden und prüfen, ob das Problem weiter besteht.

4. Annahme: Zur Wiedergabe der Musik muss der Playerprozess mit dem Treiber der Audio-Hardware (DA-Wandler) interagieren. Evtl. gibt es eintweder ein Problem bei der Kommunikation beider Prozesse oder zwischen Treiber und Hardware. \
Behebung: Ein anderes Wiedergabe-Programm testen und prüfen, ob das Problem weiter besteht. Den Treiber ggf. aktualisieren. Prüfen, ob verschiedene Treiber zu Auswahl stehen und ggf. testen.
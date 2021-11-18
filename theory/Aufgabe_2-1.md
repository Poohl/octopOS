# Aufgabe 2-1  (Threads)  – 6  Punkte

**Wann ist es sinnvoll, nebenläufige Programmteile mit Hilfe von Threads anstatt Prozessen
(heavyweight processes) zu implementieren?**

Es bietet sich an, nebenläufige Programmteile mithilfe von Threads zu implementieren, wenn viel Kommunikation zwischen diesen stattfinden muss. Bei Threads kann dies einfach über den gemeinsamen Speicherbereich passieren, während bei Prozessen die Kommunikation Kontextwechsel erfordert.

**Welche Vorteile bieten User-Level-Threads gegenüber den Kernel-Level-Threads? Gibt es
auch Nachteile?**

Zwischen den User-Level-Threads eines Kernel-Level-Threads kann schnell (ohne Kontextswitch) gewechselt werden. Für den Kernel sind diese als solche zunächst transparent, wobei auch eine 1-zu-1-Zuordnung von Kernel-Level-Thread zu User-Level-Thread (sowohl mit als auch ohne Abstraktion durch eine Laufzeitumgebung) möglich ist.
Als Nachteil kann betrachtet werden, dass die User-Level-Threads eines Kernel-Level-Threads nicht  echt parallel, d.i. auf verschiedenen Kernen, ausgeführt werden können.


**Welche Vorteile und Nachteile gibt es, wenn man Thread-Kontrollblöcke (TCB) als Skalare, in
Arrays, Listen, Bäumen oder invertierten Tabellen speichert?**

| | Vorteile | Nachteile 
| --- | --- | --- |
| Skalar | <ul><li>einfach zu implementieren</li><li>es muss kein zentraler Bereich im Speicher vorgehalten werden, der ggf. groß Genug für alle TCBs ist</li></ul> | <ul><li> keine Strukturierung </li> <li> Ablageorte müssen zur Verwaltung je einzeln bekannt sein </li></ul>
| Array | <ul><li> einfach zu implementieren </li> <li> kaum Speicher-Overhead (nur Pointer auf Anfang) </li> <li> zentraler, strukturierbarer Speicherort </li></ul> | <ul><li> Verwaltungs-Overhead beim Erzeugen/Vernichten von Threads (Lücken im Array schließen) </li>  <li> hinreichend großer Bereich im Speicher muss vorgehalten werden </li></ul> |
| Liste | <ul><li> strukturiert </li> <li> geringer Verwaltungs-Overhead (Elemente können durch Zeigermanipulation eingefügt, Verschoben oder entfernt werden) </li> <li> Speicherbereich muss nicht notwendigerweise Zusammenhängen </li> </ul> | <ul><li> höherer Speicher-Overhead (jeder TCB hat mindestens einen Zeiger) </li></ul> |
| Bäume | <ul><li> strukturiert </li> <li> je nach Art des Baums (balanciert, z.B. AVL, Rot-Schwarz etc.) sehr gute Sucheigenschaften </li> </ul> |  <ul><li> je nach speziellen Eigenschaften evtl. komplexer zu implementieren  </li> <li> höherer Speicher-Overhead </li> <li> höherer Verwaltungs-Overhead beim Einfügen/Löschen (je nach Art) </li></ul> 
| invertiere Tabelle | <ul><li> strukturiert </li> <li> vereinfacht die Verwaltung hinsichtlich spezieller Attribute </li> <li> verschiedene Attribute können in verschiedenen Adressräumen abgelegt sein </li></ul> | <ul><li> höherer Verwaltungs-Overhead beim Einfügen/Löschen (insb. wenn in unterschiedlichen Adressräumen abgelegt) </li></ul> |


**In welchem Adressraum (Prozess-Eigner, Dienste-Prozess, BS-Kern) wird ein TCB ge-
speichert?**

Es kommt drauf an …
Das TCB muss nicht zwingend zentral in einem Adressraum liegen. Wichtig ist, dass kritische Teile wie z.B. ID und Priorität nur vom BS-Kern modifiziert werden können (und im entsprechenden Adressraum liegen). Andere Daten wie etwa der Scheduling-Status (Ready, Blocked, Running) können im Adressraum des Schedulers liegen (der kein Teil des Kerns sein muss).

# Auswahlstrategien

Alle Ansätze haben im worst case bei Verwendung einer linked list von freien Speichersegmenten eine Laufzeit von O(n), da alle freien Stücke auf "fit" überprüft werden müssen. Im average case hat Best-fit (fast) immer eine Laufzeit von O(n) da man sich erst nach Überprüfung aller Optionen sicher sein kann die beste gefunden zu haben. Sowohl Next- als auch First-fit finden im bei Erfolg vor Überprüfung aller Optionen ein passendes Speichersegment, jedoch muss First-fit Erfahrungsgemäß mehr kleine Stücke zu Beginn überprüfen, da es dazu tendiert alle Anfragten an den Anfang des Speicherbereichtrs zu allokieren und dabei viele kleine unbrauchbare Fragmente zu erzeugen.

Die Laufzeit von Best-Fit lässt sich auf O(log n) durch die Verwendung eines nach Speichergröße sortierten Binärbaumes reduzieren.

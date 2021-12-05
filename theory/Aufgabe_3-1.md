* Beim **gegenseitigen Ausschluss** wird sichergestellt, dass verschiedene Teilnehmer nicht gleichzeitig auf den selben kritischen Abschnitt im Code zugreifen. Hierfür wird zur "Benutzerfreundlichkeit" auf **Signalisierung** zurückgegriffen (Locks). Es wird eine **Synchronisierung** implementiert -- ein Thread wartet, bis der kritische Abschnitt wieder freigegeben ist. Diese Funktionalität wird regelmäßig in primitiver Form (swb, ldrex, strex) direkt in HW implementiert.

* Die **Signalisierung** allgemein dient der zeitlichen (im Sinne von Abfolge nicht Realzeit) Koordination verschiedener Prozesse. Hierzu stimmen sich die beteiligten Akteure über ein Signal ab. Dies kann Speicher sein oder auch auf anderem Wege implementiert (z.B. Interrupts).

* Bei der **Synchronisierung** werden verschiedene Threads zeitlich Koordiniert, indem an einer bestimmten Stelle im Code gewartet wird, bis alle relevanten Teilnehmer ebenfalls an ihrer jeweiligen "Barriere" angelangt sind. Dieses Konzept kann auf Gruppen erweitert werden.

* **Koordination** ist der Oberbegriff für alle gewollten Interaktionen zwischen verschiedenen Threads.

* Bei der **Kommunikation** werden Daten über Sende- und Empfangsoperationen zwischen verschiedenen Teilnehmern ausgetauscht.

* Im gegensatz zur **Kommunikation** werden bei der **Kooperation** Daten zwschen verschiedenen Teilnehmern ausgetauscht, indem diese auf einen gemeinsamen Speicher zugreifen können. Dies erfordert zeitliche Koordination z.B. durch **gegenseitigen Ausschluss**.

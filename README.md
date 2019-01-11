# ess_final
## [HeardRate Click](https://cis.technikum-wien.at/documents/bic/3/ess/semesterplan/boosterpacks/heartrate_click.html)

### Allgemeines

Implementieren Sie gundsätzlich die Ansteuerung/Auswertung des HeartRate Click-Moduls. Bilden Sie mit mindestem einem passenden "Gegenstück" eine Gruppe (min. ein Output-Click und ein Input-Click).

Sehen Sie eine Ausgabe-/Ansteuermöglichkeit unabhängig vom zweiten Modul vor (beispielsweise über UART).
### Einsetzbare Technologien

*I2C, Interrupt*
### Aufgabenstellungen
Für Erreichung bis zu einem „Gut“

Pollen Sie die aktuellen Messwerte für die Pulsmessung (mit ca 50 Hz). Geben sie diese in SI-Einheiten weiter. Überlegen Sie sich eine sinnvolle pseudo-grafische Darstellung über UART, bei der man den Pulsverlauf (pro Herzschlag) erkennen kann.
Für Erreichung bis zu einem „Sehr gut“

Benützen Sie den Interrupt des HeartRate Click-Moduls, um ein ständiges Pollen zu vermeiden. Werten Sie den tatsächlichen Herzschlag aus!

Achtung! - dieser Aufgabenteil geht in Richtung Signalverarbeitung


## [OLED_C_CLick](https://cis.technikum-wien.at/documents/bic/3/ess/semesterplan/boosterpacks/oled_c_click.html)
### Einsetzbare Technologien

*SPI, Vollgrafik Display*
### Aufgabenstellungen
Für Erreichung bis zu einem „Befriedigend“

Bei diesem Modul gibt es dafür keine eingeschränkte Aufgabe.
Für Erreichung bis zu einem „Gut“

Unterstützen Sie eine mehrzeilige Textausgabe auf dem OLED.
Für Erreichung bis zu einem „Sehr gut“

Unterstützen sie automatisches Scrollen mit einer Schrittweite von einem Pixel, wenn der Text größer als der Bildschirm ist.

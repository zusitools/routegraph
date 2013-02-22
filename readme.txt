Systemvoraussetzung
===================

Zusi 2.4.7.3 (Versionsnummer muss exakt stimmen)

Legende
=======

siehe legende.pdf

Bedienung der Ansicht
=====================

Ziehen mit gedrückter linker Maustaste: Verschieben
Ziehen mit gedrückter rechter Maustaste nach oben/unten: Rotieren

Doppelklick mit rechter Maustaste: Zurücksetzen der Ansicht (Rotation und Zoom)

Mausrad: Vertikales Scrollen
Mausrad mit gedrückter Alt-Taste: Horizontales Scrollen
Mausrad mit gedrückter Strg-Taste: Zoom

Doppelklick auf einen Zug: Zug in Zusi übernehmen (nur unter Windows, nur wenn Zusi läuft und der Zug aufgegleist ist, nicht bei gespeicherten Aufzeichnungen)
Doppelklick auf einen Zug mit gedrückter Strg-Taste: Zug immer in der Fenstermitte behalten (Verfolgermodus)
Doppelklick mit linker Maustaste irgendwo ins Fenster: Verfolgermodus beenden

Züge anzeigen
=============

Züge werden automatisch angezeigt, wenn
 - Zusi läuft
 - die korrekte Strecke geladen ist
 - die Simulation gestartet wurde

Der Menüpunkt „Züge → Mit Zusi neu verbinden“ löscht die existierenden Züge aus dem Fenster und versucht eine Neuverbindung mit Zusi.

Wird ein neuer Fahrplan geladen und es sind aufgezeichnete Züge vorhanden, so wird die Aufzeichnung abgebrochen, aber nicht gelöscht. Eine Aufzeichnung des neuen Fahrplans muss explizit mit „Züge → Mit Zusi neu verbinden“ gestartet werden.

Drückt man im Streckengraph-Fenster die Tasten F2 (Pause), F9 (Zeitraffer) oder F11 (Zeitsprung), so werden diese Kommandos an Zusi weitergeleitet (nur unter Windows).

Tipps
=====

Signale und Aufgleispunkte zeigen ihren Namen als Tooltip an, wenn man mit dem Mauszeiger darüber stehen bleibt.

Aufgezeichnet wird 10x pro Sekunde echter Zeit, gespeichert wird allerdings nur eine Aufzeichnung pro Sekunde Simulationszeit, um den Speicherbedarf zu minimieren.

Die Aufzeichnung erfolgt auch, wenn Zusi im Zeitraffer- oder Zeitsprungmodus läuft. Allerdings existiert dann u.U. weniger als eine Aufzeichnung pro Sekunde Simulationszeit. Im Zeitsprungmodus kann es auch durchaus zu Inkonsistenzen in der Anzeige kommen

Sonstiges
=========

Copyrighthinweis für die verwendeten Bestandteile von LibQxt:

/****************************************************************************
** Copyright (c) 2006 - 2011, the LibQxt project.
** See the Qxt AUTHORS file for a list of authors and copyright holders.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the LibQxt project nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** <http://libqxt.org>  <foundation@libqxt.org>
*****************************************************************************/
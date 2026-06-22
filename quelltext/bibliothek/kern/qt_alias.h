#pragma once

// ---------------------------------------------------------------------------
// Deutsche Aliase für Qt-Schlüsselwörter
// ---------------------------------------------------------------------------
// Der MOC-Parser versteht nur englische Qt-Schlüsselwörter (signals, slots).
// Diese Datei definiert deutsche Aliase, damit der Quelltext deutsch bleibt,
// der Compiler aber die englischen Originale sieht.
//
// Vor jedem Qt-Header einbinden:
//   #include "kern/qt_alias.h"
// ---------------------------------------------------------------------------

#define signale             signals
#define oeffentlicheSlots   public slots
#define privateSlots        private slots

// Qt selbst braucht die Originale für Q_OBJECT-Makros —
// daher NUR in Projekt-Headern verwenden.

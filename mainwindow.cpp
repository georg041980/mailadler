#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Sprachauswahl mit festen Namen (kein tr()!)
    ui->sprachBox->addItem("Deutsch", "de_DE");
    ui->sprachBox->addItem("English", "en_US");
    ui->sprachBox->addItem("Español", "es_ES");

    // Letzte gespeicherte Sprache laden, sonst Deutsch
    QSettings einstellungen("Mailadler", "Mailadler");
    QString gespeicherteSprache = einstellungen.value("Sprache", "de_DE").toString();
    int index = ui->sprachBox->findData(gespeicherteSprache);
    if (index >= 0)
        ui->sprachBox->setCurrentIndex(index);
    else
        ui->sprachBox->setCurrentIndex(0);

    // Initiale Sprache laden
    spracheLaden(gespeicherteSprache);

    // Verbindung: Wenn Dropdown geändert wird, Sprache wechseln
    connect(ui->sprachBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::spracheWechseln);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::spracheWechseln(int index)
{
    QString neueSprache = ui->sprachBox->itemData(index).toString();

    // In Einstellungen speichern
    QSettings einstellungen("Mailadler", "Mailadler");
    einstellungen.setValue("Sprache", neueSprache);

    // Sprache laden und GUI komplett aktualisieren
    spracheLaden(neueSprache);
    guiNeuUebersetzen();
}

void MainWindow::spracheLaden(const QString &sprachCode)
{
    // Alten Übersetzer entfernen
    qApp->removeTranslator(&aktuellerUebersetzer);

    // Neue .qm-Datei laden (aus der Ressource)
    QString pfad = ":/translations/mailadler_" + sprachCode + ".qm";
    bool geladen = aktuellerUebersetzer.load(pfad);
    if (!geladen)
        qWarning() << "Übersetzung nicht geladen:" << pfad;
    else
        qApp->installTranslator(&aktuellerUebersetzer);
}

void MainWindow::guiNeuUebersetzen()
{
    // Vom Designer erzeugte Texte aktualisieren (Menüs, Fenstertitel etc.)
    ui->retranslateUi(this);

    // Ordnerliste neu aufbauen – jetzt in der neuen Sprache
    ui->ordnerListe->clear();
    ui->ordnerListe->addItem(tr("Posteingang"));
    ui->ordnerListe->addItem(tr("Gesendet"));
    ui->ordnerListe->addItem(tr("Entwürfe"));
    ui->ordnerListe->addItem(tr("Papierkorb"));
    ui->ordnerListe->addItem(tr("Spam"));
    ui->ordnerListe->addItem(tr("Archiv"));

    // ComboBox bleibt unverändert – sie zeigt immer "Deutsch", "English", "Español"
}
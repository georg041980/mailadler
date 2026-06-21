#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->ordnerListe->addItem(tr("Posteingang"));
    ui->ordnerListe->addItem(tr("Gesendet"));
    ui->ordnerListe->addItem(tr("Entwürfe"));
    ui->ordnerListe->addItem(tr("Papierkorb"));
    ui->ordnerListe->addItem(tr("Spam"));
    ui->ordnerListe->addItem(tr("Archiv"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


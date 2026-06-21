#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void spracheWechseln(int index);

private:
    Ui::MainWindow *ui;
    QTranslator aktuellerUebersetzer;

    void spracheLaden(const QString &sprachCode);
    void guiNeuUebersetzen();
};

#endif // MAINWINDOW_H
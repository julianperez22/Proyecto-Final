#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>  // 👈 Escucha del teclado
#include "personaje.h" // 👈 Nueva clase balsa

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *evento) override; // 👈 Captura pulsaciones

private slots:
    void animarMarea();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *escena;
    QGraphicsView *vista;

    QGraphicsPixmapItem *objetoFondo;
    Personaje *balsaItem; // 👈 El objeto de nuestro bote jugable
    QTimer *timerMarea;
    double tiempo;
};

#endif // MAINWINDOW_H

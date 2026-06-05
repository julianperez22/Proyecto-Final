#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>
#include <vector>

#include "personaje.h"
#include "obstaculo.h"
#include "disco.h"
#include "gaviota.h"

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
    void keyPressEvent(QKeyEvent *evento) override;
    void keyReleaseEvent(QKeyEvent *evento) override;

private slots:
    void animarMarea();
    void aumentarDificultad();
    void generarObstaculo();
    void generarGaviota();
    void crearDisco(double posX, double posY, bool haciaDerecha);

private:
    void reiniciarJuego();
    void detenerTimers();
    void mostrarGameOver(const QString& mensaje);

    Ui::MainWindow *ui;
    QGraphicsScene *escena;
    QGraphicsView *vista;
    Personaje *balsaItem;

    std::vector<QGraphicsPixmapItem*> listaFondos;
    std::vector<bool> esEspejoFondo;

    const int ANCHO_FONDO = 1600;
    const int CANTIDAD_FONDOS = 4;

    std::vector<Obstaculo*> listaObstaculos;
    std::vector<Disco*> listaDiscos;
    std::vector<Gaviota*> listaGaviotas;

    QPixmap texturaPiedra;

    QTimer *timerMarea;
    QTimer *timerDificultad;
    QTimer *timerSpawnObstaculos;
    QTimer *timerSpawnGaviotas;

    double tiempo;
    double velocidadFondo;
    int segundosTranscurridos;

    bool juegoTerminado;
    QGraphicsTextItem* textoGameOverItem;
};

#endif // MAINWINDOW_H

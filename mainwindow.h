#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <vector>

#include "personaje.h"
#include "obstaculo.h"
#include "disco.h"
#include "gaviota.h"
#include "caca.h"
#include "jefe.h"
#include "balaagua.h"

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
    void mousePressEvent(QMouseEvent *evento) override;

private slots:
    void animarMarea();
    void aumentarDificultad();
    void generarObstaculo();
    void generarGaviota();
    void crearDisco(double posX, double posY, bool haciaDerecha);
    void agregarCaca(Caca* caca);
    void spawnearJefe();
    void crearBalaAgua(qreal x, qreal y);
    void spawnearJefeNivel2();
    void crearBalaAguaNivel2(qreal x, qreal y);
    void ocultarMensajeNivelCompletado();

private:
    void reiniciarJuego();
    void detenerTimers();
    void mostrarGameOver(const QString& mensaje);
    void actualizarInterfazVidas();
    void mostrarMenuInicial();

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
    std::vector<Caca*> listaCacas;
    std::vector<BalaAgua*> listaBalasAgua;

    QPixmap texturaPiedra;

    QTimer *timerMarea;
    QTimer *timerDificultad;
    QTimer *timerSpawnObstaculos;
    QTimer *timerSpawnGaviotas;
    QTimer *timerSpawnJefe;
    QTimer *timerNivel2Jefe;

    double tiempo;
    double velocidadFondo;
    int segundosTranscurridos;

    bool juegoTerminado;
    QGraphicsTextItem* textoGameOverItem;
    QGraphicsTextItem* textoVidasItem;
    QGraphicsTextItem* textoNivelCompletado;

    // Menú
    QGraphicsTextItem* menuTitulo;
    QGraphicsTextItem* menuIniciar;
    QGraphicsTextItem* menuSalir;
    bool menuActivo = true;

    // Jefe y Niveles
    Jefe* jefeActual = nullptr;
    int nivelActual = 1;
};

#endif // MAINWINDOW_H

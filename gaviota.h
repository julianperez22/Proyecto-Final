#ifndef GAVIOTA_H
#define GAVIOTA_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QObject>
#include <QTimer>

class Personaje;
class Caca; // Declaración adelantada para evitar dependencias circulares

class Gaviota : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Gaviota(QGraphicsScene *escenaPadre, Personaje *jugador);

    void actualizarPosicion(double velocidadJuego);

signals:
    void cacaLanzada(Caca *nuevaCaca); // <-- Nueva señal para avisar a MainWindow

private slots:
    void tirarCaca();

private:
    QGraphicsScene *escena;
    Personaje *balsaItem;
    QTimer *timerTirar;
    double velocidad;
};

#endif // GAVIOTA_H

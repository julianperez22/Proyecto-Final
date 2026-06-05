#ifndef GAVIOTA_H
#define GAVIOTA_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QObject>

class Gaviota : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Gaviota(QGraphicsScene *escenaPadre);

    void actualizarPosicion(double velocidadJuego);

private:
    QGraphicsScene *escena;
    double velocidad;
};

#endif // GAVIOTA_H

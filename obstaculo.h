#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

class Obstaculo : public QGraphicsPixmapItem
{
public:

    Obstaculo(const QPixmap &pixmap, QGraphicsScene *escenaPadre);

    void actualizarPosicion(double velocidadActual);

private:
    QGraphicsScene *escena;
};

#endif // OBSTACULO_H

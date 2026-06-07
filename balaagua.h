#ifndef BALAAGUA_H
#define BALAAGUA_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <qmath.h>

class BalaAgua : public QGraphicsPixmapItem
{
public:
    BalaAgua(QGraphicsScene *escenaPadre, qreal startX, qreal startY);

    void actualizar();
    void setMovimientoSenoidal(bool usarSeno);

private:
    QGraphicsScene *escena;
    double tiempo = 0.0;
    double velocidadX = -4.8;
    double amplitud = 50.0;
    double frecuencia = 0.09;
    double posicionYBase;
    bool usarSeno = true;
};

#endif // BALAAGUA_H

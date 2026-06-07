#ifndef CACA_H
#define CACA_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

class Personaje;

class Caca : public QGraphicsItem
{
public:
    Caca(QGraphicsScene *escenaPadre, qreal startX, qreal startY, Personaje *jugador);

    void actualizar();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QGraphicsScene *escena;
    Personaje *balsaItem;
    double velocidadCaida = 7.5;
};

#endif // CACA_H

#include "caca.h"
#include "personaje.h"
#include <QPainter>

Caca::Caca(QGraphicsScene *escenaPadre, qreal startX, qreal startY, Personaje *jugador)
    : escena(escenaPadre), balsaItem(jugador)
{
    setPos(startX, startY);
    setZValue(15);
    escena->addItem(this);
}

void Caca::actualizar()
{
    if (scene() == nullptr) return;

    // Caída en línea recta
    setPos(x(), y() + velocidadCaida);
}

QRectF Caca::boundingRect() const
{
    return QRectF(-15, -15, 30, 35);
}

void Caca::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QBrush(QColor(101, 67, 33)));
    painter->setPen(QPen(QColor(54, 35, 18), 1.5));
    painter->drawEllipse(-10, -5, 20, 20);
    painter->drawEllipse(-6, -12, 12, 12);
}

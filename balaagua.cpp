#include "balaagua.h"

BalaAgua::BalaAgua(QGraphicsScene *escenaPadre, qreal startX, qreal startY)
    : escena(escenaPadre)
{
    QPixmap pix(":/bala_agua.png");

    if (!pix.isNull()) {
        setPixmap(pix.scaled(32, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        QPixmap fallback(28, 16);
        fallback.fill(Qt::cyan);
        setPixmap(fallback);
    }

    setPos(startX, startY);
    setZValue(18);
    escena->addItem(this);

    posicionYBase = startY;
}

void BalaAgua::setMovimientoSenoidal(bool usarSeno)
{
    this->usarSeno = usarSeno;
}

void BalaAgua::actualizar()
{
    if (scene() == nullptr) return;

    tiempo += frecuencia;

    double nuevaX = x() + velocidadX;
    double offsetY = usarSeno ? qSin(tiempo) * amplitud
                              : qCos(tiempo) * amplitud;

    setPos(nuevaX, posicionYBase + offsetY);
}

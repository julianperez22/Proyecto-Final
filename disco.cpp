#include "disco.h"

Disco::Disco(QGraphicsScene *escenaPadre, bool haciaIzquierda)
    : QGraphicsPixmapItem(), escena(escenaPadre), velocidad(18.0), haciaIzquierda(haciaIzquierda)
{
    QPixmap pixmap(":/disco.png");   // Cambia a ":/disco.png" cuando tengas el sprite
    setPixmap(pixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    setZValue(10);
}

void Disco::actualizarPosicion()
{
    if (!escena) return;

    if (haciaIzquierda) {
        setPos(x() - velocidad, y());
    } else {
        setPos(x() + velocidad, y());
    }

    // Eliminamos el disco ANTES de que salga completamente de la pantalla
    if (x() < -50 || x() > 1700) {
        if (escena && scene()) {
            escena->removeItem(this);
        }
        delete this;
    }
}

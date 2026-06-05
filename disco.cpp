#include "disco.h"

Disco::Disco(QGraphicsScene *escenaPadre, bool haciaIzquierda)
    : escena(escenaPadre), haciaIzquierda(haciaIzquierda), velocidad(18.0)
{
    QPixmap pixmap(":/disco.png");
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

    // Eliminar de forma segura cuando sale de pantalla
    if (x() < -100 || x() > 1750) {
        if (escena) {
            escena->removeItem(this);
        }
        delete this;
    }
}

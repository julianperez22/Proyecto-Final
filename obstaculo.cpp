#include "obstaculo.h"
#include <QRandomGenerator>

Obstaculo::Obstaculo(const QPixmap &pixmap, QGraphicsScene *escenaPadre)
    : escena(escenaPadre)
{
    setPixmap(pixmap);


    int limiteSuperiorY = 420;    // Para ajustar donde salen las piedras
    int limiteInferiorY = 680;    //

    int randomY = QRandomGenerator::global()->bounded(limiteSuperiorY, limiteInferiorY);

    setPos(1600, randomY);

    escena->addItem(this);
}

void Obstaculo::actualizarPosicion(double velocidadActual)
{
    setPos(x() - velocidadActual, y());
}

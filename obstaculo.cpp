#include "obstaculo.h"
#include <QRandomGenerator>

Obstaculo::Obstaculo(const QPixmap &pixmap, QGraphicsScene *escenaPadre)
    : escena(escenaPadre)
{

    setPixmap(pixmap);


    int limiteSuperiorY = 320;
    int limiteInferiorY = 620;
    int randomY = QRandomGenerator::global()->bounded(limiteSuperiorY, limiteInferiorY);


    setPos(1280, randomY);

    escena->addItem(this);
}

void Obstaculo::actualizarPosicion(double velocidadActual)
{
    setPos(x() - velocidadActual, y());
}

#include "obstaculo.h"
#include <QRandomGenerator>

Obstaculo::Obstaculo(const QPixmap &pixmap, QGraphicsScene *escenaPadre)
    : escena(escenaPadre)
{
    setPixmap(pixmap);

    // === LÍMITES AJUSTADOS PARA QUE LAS PIEDRAS SALGAN MÁS ABAJO ===
    int limiteSuperiorY = 420;    // ← Antes estaba en 320, ahora más abajo
    int limiteInferiorY = 680;    // ← Límite inferior (cerca del fondo)

    int randomY = QRandomGenerator::global()->bounded(limiteSuperiorY, limiteInferiorY);

    setPos(1600, randomY);        // ← También actualizado para la ventana más grande

    escena->addItem(this);
}

void Obstaculo::actualizarPosicion(double velocidadActual)
{
    setPos(x() - velocidadActual, y());
}

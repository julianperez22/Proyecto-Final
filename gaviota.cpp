#include "gaviota.h"
#include "caca.h"
#include <QDebug>

Gaviota::Gaviota(QGraphicsScene *escenaPadre, Personaje *jugador)
    : QObject(), QGraphicsPixmapItem(), escena(escenaPadre), balsaItem(jugador), velocidad(7.5)
{
    QPixmap pix密(":/gaviota.png");
    if (!pix密.isNull()) {
        setPixmap(pix密.scaled(105, 62, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        QPixmap fallback密(":/piedra.png");
        setPixmap(fallback密.scaled(100, 60));
    }

    setPos(1620, 130);
    setZValue(20);

    timerTirar = new QTimer(this);
    connect(timerTirar, &QTimer::timeout, this, &Gaviota::tirarCaca);
    timerTirar->start(700);   // Cada 2 segundos intenta disparar

    escena->addItem(this);
}

void Gaviota::tirarCaca()
{
    if (scene() == nullptr || balsaItem == nullptr) return;

    // Instanciamos el proyectil
    Caca *nueva = new Caca(escena, x() + 30, y() + 35, balsaItem);

    // Emitimos la señal para que MainWindow lo capture y controle
    emit cacaLanzada(nueva);
}

void Gaviota::actualizarPosicion(double velocidadJuego)
{
    setPos(x() - (velocidad + velocidadJuego), y());
}

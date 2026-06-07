#include "jefe.h"
#include "personaje.h"
#include <QDebug>

Jefe::Jefe(QGraphicsScene *escenaPadre, Personaje *jugador)
    : QObject(), QGraphicsPixmapItem()
{
    escena = escenaPadre;
    balsaItem = jugador;

    QPixmap pix(":/jefe_agua.png");
    if (!pix.isNull()) {
        setPixmap(pix.scaled(140, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        QPixmap fallback(140, 140);
        fallback.fill(Qt::red);
        setPixmap(fallback);
    }

    setPos(1440, 350);
    setZValue(30);
    escena->addItem(this);

    timerDisparo = new QTimer(this);
    connect(timerDisparo, &QTimer::timeout, this, &Jefe::disparar);
    timerDisparo->start(2000);
}

Jefe::~Jefe()
{
    if (timerDisparo) timerDisparo->stop();
}

void Jefe::disparar()
{
    if (!escena || !estaVivo()) return;
    emit disparoAgua(x() + 30, y() + 70);
    qDebug() << "💧 Jefe disparó bala de agua";
}

void Jefe::actualizar(double velocidadJuego) { Q_UNUSED(velocidadJuego); }
void Jefe::recibirDanio(int cantidad) { vida -= cantidad; }
int Jefe::obtenerVida() const { return vida; }
bool Jefe::estaVivo() const { return vida > 0; }

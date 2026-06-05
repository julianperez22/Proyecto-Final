#include "gaviota.h"
#include <QDebug>

Gaviota::Gaviota(QGraphicsScene *escenaPadre)
    : QObject(), QGraphicsPixmapItem(), escena(escenaPadre), velocidad(7.5)
{
    QPixmap pix(":/gaviota.png");
    if (!pix.isNull()) {
        setPixmap(pix.scaled(150, 92, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        qDebug() << "✅ Gaviota cargada";
    } else {
        qDebug() << "❌ No se pudo cargar gaviota.png";
        QPixmap fallback(":/piedra.png");
        setPixmap(fallback.scaled(100, 60));
    }

    setPos(1620, 130);
    setZValue(20);

    escena->addItem(this);
    qDebug() << "🕊️ Gaviota spawneada";
}

void Gaviota::actualizarPosicion(double velocidadJuego)
{
    if (scene() == nullptr) return;

    setPos(x() - (velocidad + velocidadJuego * 0.55), y());

    if (x() < -30) {
        if (escena && scene()) {
            escena->removeItem(this);
        }
        deleteLater();        // ← Uso correcto y seguro
    }
}

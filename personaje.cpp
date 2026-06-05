#include "personaje.h"
#include <QPainter>
#include <QLinearGradient>
#include <QImage>
#include <QDebug>

Personaje::Personaje(QGraphicsItem *parent)
    : QGraphicsItem(parent), velocidad(12),
    filaActual(0), columnaActual(0), mirandoIzquierda(false),
    estaMoviendo(false), escalaSprite(1.6)
{
    QImage imagenSprite;
    if (imagenSprite.load(":/sprites_naufrago.png")) {
        imagenSprite = imagenSprite.convertToFormat(QImage::Format_ARGB32);

        for (int y = 0; y < imagenSprite.height(); ++y) {
            for (int x = 0; x < imagenSprite.width(); ++x) {
                QColor c = imagenSprite.pixelColor(x, y);
                if (c.red() < 30 && c.green() < 30 && c.blue() < 30) {
                    imagenSprite.setPixelColor(x, y, Qt::transparent);
                }
            }
        }

        spriteSheet = QPixmap::fromImage(imagenSprite);
        qDebug() << "✅ Spritesheet cargado:" << spriteSheet.width() << "x" << spriteSheet.height();
    } else {
        qDebug() << "❌ No se pudo cargar ':/sprites_naufrago.png'";
    }

    setPos(100, 450);

    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &Personaje::actualizarFotograma);
}

void Personaje::actualizarFotograma()
{
    if (!estaMoviendo) return;
    columnaActual = (columnaActual + 1) % TOTAL_FRAMES;
    update();
}

void Personaje::actualizarEstadoAnimacion()
{
    if (!teclasPresionadas.isEmpty()) {
        if (!timerAnimacion->isActive()) timerAnimacion->start(100);
        estaMoviendo = true;
    } else {
        timerAnimacion->stop();
        estaMoviendo = false;
        columnaActual = 0;
    }
    update();
}

void Personaje::detenerMovimiento()
{
    teclasPresionadas.clear();
    actualizarEstadoAnimacion();
    filaActual = 0;
}

// ====================== MOVIMIENTOS ======================

void Personaje::moverIzquierda()
{
    if (x() > 10) setPos(x() - velocidad, y());
    mirandoIzquierda = true;
    filaActual = 1;
    teclasPresionadas.insert(Qt::Key_Left);
    teclasPresionadas.insert(Qt::Key_A);
    actualizarEstadoAnimacion();
}

void Personaje::moverDerecha()
{
    if (x() < 1450) setPos(x() + velocidad, y());   // Ajustado para ventana más grande
    mirandoIzquierda = false;
    filaActual = 1;
    teclasPresionadas.insert(Qt::Key_Right);
    teclasPresionadas.insert(Qt::Key_D);
    actualizarEstadoAnimacion();
}

void Personaje::moverArriba()
{
    if (y() > 300) setPos(x(), y() - velocidad);
    filaActual = 0;
    teclasPresionadas.insert(Qt::Key_Up);
    teclasPresionadas.insert(Qt::Key_W);
    actualizarEstadoAnimacion();
}

void Personaje::moverAbajo()
{
    if (y() < 650) setPos(x(), y() + velocidad);
    filaActual = 0;
    teclasPresionadas.insert(Qt::Key_Down);
    teclasPresionadas.insert(Qt::Key_S);
    actualizarEstadoAnimacion();
}

void Personaje::aplicarFisicasMarea(double empujeX, double empujeY)
{
    setPos(x() + empujeX, y() + empujeY);

    if (x() < 10) setPos(10, y());
    if (x() > 1450) setPos(1450, y());
    if (y() < 300) setPos(x(), 300);
    if (y() > 650) setPos(x(), 650);

    if (teclasPresionadas.isEmpty()) {
        filaActual = 0;
        columnaActual = 0;
    }
}

// ====================== DIBUJO ======================

QRectF Personaje::boundingRect() const
{
    return QRectF(0, 0, ANCHO_SPRITE * escalaSprite, ALTO_SPRITE * escalaSprite);
}

void Personaje::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    // === BALSA ===
    double escalaBalsa = escalaSprite * 1.25;
    double balsaW = ANCHO_SPRITE * escalaBalsa;
    double balsaTop = ALTO_SPRITE * escalaSprite - 22;

    QLinearGradient gradienteMadera(0, balsaTop, 0, balsaTop + 32);
    gradienteMadera.setColorAt(0.0, QColor(139, 90, 43));
    gradienteMadera.setColorAt(0.5, QColor(101, 67, 33));
    gradienteMadera.setColorAt(1.0, QColor(54, 35, 18));

    painter->setBrush(gradienteMadera);
    painter->setPen(QPen(QColor(40, 25, 10), 2));
    painter->drawRoundedRect(2, balsaTop, balsaW - 4, 12, 6, 6);
    painter->drawRoundedRect(0, balsaTop + 11, balsaW, 13, 6, 6);
    painter->drawRoundedRect(3, balsaTop + 22, balsaW - 6, 10, 5, 5);

    // Cuerda
    painter->setPen(QPen(QColor(210, 180, 140), 1.5));
    painter->drawLine(5, balsaTop + 12, balsaW - 5, balsaTop + 12);

    // === SPRITE ===
    if (!spriteSheet.isNull()) {
        int srcX = columnaActual * ANCHO_SPRITE;
        int srcY = filaActual   * ALTO_SPRITE;

        double destW = ANCHO_SPRITE * escalaSprite;
        double destH = ALTO_SPRITE  * escalaSprite;

        double destX = (balsaW - destW) / 2.0;
        double destY = balsaTop - destH + 24;

        painter->save();

        if (mirandoIzquierda) {
            painter->translate(destX + destW, destY);
            painter->scale(-1, 1);
            painter->drawPixmap(0, 0, destW, destH, spriteSheet, srcX, srcY, ANCHO_SPRITE, ALTO_SPRITE);
        } else {
            painter->drawPixmap(destX, destY, destW, destH, spriteSheet, srcX, srcY, ANCHO_SPRITE, ALTO_SPRITE);
        }

        painter->restore();
    }
}

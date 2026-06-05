#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>
#include <QTimer>
#include <QSet>

class Personaje : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Personaje(QGraphicsItem *parent = nullptr);

    void moverIzquierda();
    void moverDerecha();
    void moverArriba();
    void moverAbajo();
    void detenerMovimiento();
    void aplicarFisicasMarea(double empujeX, double empujeY);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private slots:
    void actualizarFotograma();

private:
    void actualizarEstadoAnimacion();

    int velocidad;
    QPixmap spriteSheet;
    QTimer *timerAnimacion;

    // === VALORES ACTUALIZADOS SEGÚN TU SPRITESHEET ===
    const int ANCHO_SPRITE  = 62;
    const int ALTO_SPRITE   = 123;
    const int TOTAL_FRAMES  = 8;

    int filaActual;
    int columnaActual;
    bool mirandoIzquierda;

    bool estaMoviendo = false;
    QSet<int> teclasPresionadas;
    double escalaSprite = 0.1;
};

#endif // PERSONAJE_H

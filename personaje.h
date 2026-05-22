#ifndef PERSONAJE_H
#define PERSONAJE_H

#include <QGraphicsPixmapItem>
#include <QObject>

class Personaje : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Personaje(QGraphicsItem *parent = nullptr);


    void moverIzquierda();
    void moverDerecha();
    void moverArriba();
    void moverAbajo();


    void aplicarFisicasMarea(double empujeX, double empujeY);

private:
    int velocidad;
};

#endif // PERSONAJE_H

#ifndef JEFE_H
#define JEFE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QTimer>

class Personaje;

class Jefe : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Jefe(QGraphicsScene *escenaPadre, Personaje *jugador);
    ~Jefe();

    void actualizar(double velocidadJuego);
    void recibirDanio(int cantidad);
    int obtenerVida() const;
    bool estaVivo() const;

signals:
    void disparoAgua(qreal x, qreal y);

private slots:
    void disparar();

private:
    QGraphicsScene *escena;
    Personaje *balsaItem;
    int vida = 4;
    QTimer *timerDisparo;
};

#endif // JEFE_H

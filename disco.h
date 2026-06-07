#ifndef DISCO_H
#define DISCO_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>

class Disco : public QGraphicsPixmapItem
{
public:
    Disco(QGraphicsScene *escenaPadre, bool haciaIzquierda = false);

    void actualizarPosicion();

private:
    QGraphicsScene *escena;
    double velocidad;
    bool haciaIzquierda;
    bool debeEliminarse() const;
};


#endif // DISCO_H

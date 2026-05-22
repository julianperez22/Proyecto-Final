#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>
#include <QPixmap>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    setWindowTitle("Ultimate Náufrago - El Mar y la Balsa");
    resize(1280, 720);
    setWindowIcon(QIcon(":/logo.png"));


    escena = new QGraphicsScene(0, 0, 1280, 720, this);


    QPixmap imagenOriginal(":/fondo_mar.png");

    QPixmap imagenEscalada = imagenOriginal.scaled(1340, 720, Qt::IgnoreAspectRatio, Qt::FastTransformation);
    objetoFondo = new QGraphicsPixmapItem(imagenEscalada);
    objetoFondo->setPos(-30, 0);
    escena->addItem(objetoFondo);


    balsaItem = new Personaje();
    escena->addItem(balsaItem);


    vista = new QGraphicsView(escena, this);
    vista->setGeometry(0, 0, 1280, 720);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setRenderHint(QPainter::Antialiasing, false);


    tiempo = 0.0;
    timerMarea = new QTimer(this);
    connect(timerMarea, &QTimer::timeout, this, &MainWindow::animarMarea);
    timerMarea->start(30);

    vista->show();
}


void MainWindow::animarMarea()
{
    tiempo += 0.04;


    double desplazamientoFondoX = -30.0 + (std::sin(tiempo) * 25.0);
    double desplazamientoFondoY = std::cos(tiempo) * 4.0;
    objetoFondo->setPos(desplazamientoFondoX, desplazamientoFondoY);

    //Fisicas
    double fuerzaMareaX = std::sin(tiempo) * 1.2;
    double fuerzaCorrienteY = 0.4;

    balsaItem->aplicarFisicasMarea(fuerzaMareaX, fuerzaCorrienteY);
}


void MainWindow::keyPressEvent(QKeyEvent *evento)
{
    if (evento->key() == Qt::Key_Left || evento->key() == Qt::Key_A) {
        balsaItem->moverIzquierda();
    }
    else if (evento->key() == Qt::Key_Right || evento->key() == Qt::Key_D) {
        balsaItem->moverDerecha();
    }
    else if (evento->key() == Qt::Key_Up || evento->key() == Qt::Key_W) {
        balsaItem->moverArriba();
    }
    else if (evento->key() == Qt::Key_Down || evento->key() == Qt::Key_S) {
        balsaItem->moverAbajo();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

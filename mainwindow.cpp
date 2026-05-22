#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <cmath>
#include <QDebug>
#include <QRandomGenerator>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Ultimate Náufrago - Mar y Obstáculos");
    resize(1280, 720);
    setWindowIcon(QIcon(":/logo.png"));

    escena = new QGraphicsScene(0, 0, 1280, 720, this);

    // Preparar fondos
    QImage imagenBase(":/fondo_mar.png");
    QImage imagenNormal = imagenBase.scaled(ANCHO_FONDO, 720, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage imagenEspejo = imagenNormal.flipped(Qt::Horizontal);
    QPixmap pixmapNormal = QPixmap::fromImage(imagenNormal);
    QPixmap pixmapEspejo = QPixmap::fromImage(imagenEspejo);

    for (int i = 0; i < CANTIDAD_FONDOS; ++i) {
        QGraphicsPixmapItem* nuevoFondo;
        bool invertido = (i % 2 != 0);
        if (invertido) nuevoFondo = new QGraphicsPixmapItem(pixmapEspejo);
        else nuevoFondo = new QGraphicsPixmapItem(pixmapNormal);

        int posX = i * (ANCHO_FONDO - 1);
        nuevoFondo->setPos(posX, 0);
        escena->addItem(nuevoFondo);
        listaFondos.push_back(nuevoFondo);
        esEspejoFondo.push_back(invertido);
    }

    balsaItem = new Personaje();
    escena->addItem(balsaItem);

    vista = new QGraphicsView(escena, this);
    vista->setGeometry(0, 0, 1280, 720);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setRenderHint(QPainter::Antialiasing, true);
    vista->setRenderHint(QPainter::SmoothPixmapTransform, true);

    tiempo = 0.0;
    segundosTranscurridos = 0;
    velocidadFondo = 2.0;
    juegoTerminado = false;
    textoGameOverItem = nullptr;


    QPixmap pixmapOriginal(":/piedra.png");
    texturaPiedra = pixmapOriginal.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);


    timerMarea = new QTimer(this);
    connect(timerMarea, &QTimer::timeout, this, &MainWindow::animarMarea);
    timerMarea->start(16);

    timerDificultad = new QTimer(this);
    connect(timerDificultad, &QTimer::timeout, this, &MainWindow::aumentarDificultad);
    timerDificultad->start(10000);

    timerSpawnObstaculos = new QTimer(this);
    connect(timerSpawnObstaculos, &QTimer::timeout, this, &MainWindow::generarObstaculo);
    timerSpawnObstaculos->start(2500);

    vista->show();
}

void MainWindow::aumentarDificultad()
{
    segundosTranscurridos += 10;
    velocidadFondo += 1.2;
}

void MainWindow::generarObstaculo()
{
    if (QRandomGenerator::global()->bounded(0, 100) < 70) {
        Obstaculo* nuevaPiedra = new Obstaculo(texturaPiedra, escena);
        listaObstaculos.push_back(nuevaPiedra);
    }
}

void MainWindow::animarMarea()
{
    tiempo += 0.02;
    double desplazamientoY = std::cos(tiempo) * 2.0;
    int posY = static_cast<int>(desplazamientoY);


    for (size_t i = 0; i < listaFondos.size(); ++i) {
        listaFondos[i]->setPos(listaFondos[i]->x() - velocidadFondo, posY);
    }

    if (listaFondos.front()->x() <= -ANCHO_FONDO) {
        QGraphicsPixmapItem* fondoSaliente = listaFondos.front();
        listaFondos.erase(listaFondos.begin());
        bool espejoSaliente = esEspejoFondo.front();
        esEspejoFondo.erase(esEspejoFondo.begin());

        QGraphicsPixmapItem* ultimoFondoActual = listaFondos.back();
        double nuevaX = ultimoFondoActual->x() + (ANCHO_FONDO - 1);
        fondoSaliente->setPos(nuevaX, posY);

        listaFondos.push_back(fondoSaliente);
        esEspejoFondo.push_back(espejoSaliente);
    }


    for (auto it = listaObstaculos.begin(); it != listaObstaculos.end(); ) {
        Obstaculo* piedra = *it;
        piedra->actualizarPosicion(velocidadFondo);


        if (balsaItem->collidesWithItem(piedra)) {
            juegoTerminado = true;

            timerMarea->stop();
            timerDificultad->stop();
            timerSpawnObstaculos->stop();


            textoGameOverItem = new QGraphicsTextItem("GAME OVER\n[ Presiona Enter para reiniciar ]");
            QFont fuenteGame("Arial", 40, QFont::Bold);
            textoGameOverItem->setFont(fuenteGame);
            textoGameOverItem->setDefaultTextColor(Qt::red);
            textoGameOverItem->setTextWidth(800);


            textoGameOverItem->setPos(290, 260);
            escena->addItem(textoGameOverItem);

            qDebug() << "💥 ¡Colisión detectada! Fin del juego. Presiona Enter para reiniciar.";
            return;
        }

        if (piedra->x() < -60) {
            escena->removeItem(piedra);
            delete piedra;
            it = listaObstaculos.erase(it);
        } else {
            ++it;
        }
    }


    double factorDificultad = velocidadFondo / 2.0;
    double fuerzaMareaX = (-0.2 * factorDificultad) + (std::sin(tiempo) * 0.5);
    double fuerzaCorrienteY = std::cos(tiempo) * 0.15;
    balsaItem->aplicarFisicasMarea(fuerzaMareaX, fuerzaCorrienteY);
}

void MainWindow::keyPressEvent(QKeyEvent *evento)
{

    if (juegoTerminado) {
        if (evento->key() == Qt::Key_Return || evento->key() == Qt::Key_Enter) {
            reiniciarJuego();
        }
        return;
    }


    if (evento->key() == Qt::Key_Left || evento->key() == Qt::Key_A) balsaItem->moverIzquierda();
    else if (evento->key() == Qt::Key_Right || evento->key() == Qt::Key_D) balsaItem->moverDerecha();
    else if (evento->key() == Qt::Key_Up || evento->key() == Qt::Key_W) balsaItem->moverArriba();
    else if (evento->key() == Qt::Key_Down || evento->key() == Qt::Key_S) balsaItem->moverAbajo();
}


void MainWindow::reiniciarJuego()
{
    qDebug() << "🔄 Reiniciando el mar y limpiando rocas...";


    for (Obstaculo* piedra : listaObstaculos) {
        escena->removeItem(piedra);
        delete piedra;
    }
    listaObstaculos.clear();


    if (textoGameOverItem != nullptr) {
        escena->removeItem(textoGameOverItem);
        delete textoGameOverItem;
        textoGameOverItem = nullptr;
    }


    balsaItem->setPos(100, 450);


    tiempo = 0.0;
    segundosTranscurridos = 0;
    velocidadFondo = 2.0;
    juegoTerminado = false;


    timerMarea->start(16);
    timerDificultad->start(10000);
    timerSpawnObstaculos->start(2500);
}

MainWindow::~MainWindow()
{
    for (Obstaculo* piedra : listaObstaculos) delete piedra;
    listaObstaculos.clear();
    delete ui;
}

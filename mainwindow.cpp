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
    resize(1600, 900);
    setWindowIcon(QIcon(":/logo.png"));

    escena = new QGraphicsScene(0, 0, 1600, 900, this);

    // Fondos
    QImage imagenBase(":/fondo_mar.png");
    QImage imagenNormal = imagenBase.scaled(ANCHO_FONDO, 900, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
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

    connect(balsaItem, &Personaje::discoLanzado, this, &MainWindow::crearDisco);

    vista = new QGraphicsView(escena, this);
    vista->setGeometry(0, 0, 1600, 900);
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

    timerSpawnGaviotas = new QTimer(this);
    connect(timerSpawnGaviotas, &QTimer::timeout, this, &MainWindow::generarGaviota);
    timerSpawnGaviotas->start(10000);

    vista->show();
}

void MainWindow::aumentarDificultad()
{
    if (juegoTerminado) return;

    segundosTranscurridos += 10;
    velocidadFondo += 1.2;
    qDebug() << "⚡ Dificultad aumentada - Velocidad ahora:" << velocidadFondo;
}

void MainWindow::generarGaviota()
{
    if (juegoTerminado) return;

    Gaviota* nueva = new Gaviota(escena);
    listaGaviotas.push_back(nueva);
}

void MainWindow::crearDisco(double posX, double posY, bool haciaDerecha)
{
    if (juegoTerminado) return;
    Disco* nuevoDisco = new Disco(escena, !haciaDerecha);
    nuevoDisco->setPos(posX, posY);
    escena->addItem(nuevoDisco);
    listaDiscos.push_back(nuevoDisco);
}

void MainWindow::generarObstaculo()
{
    if (juegoTerminado) return;
    if (QRandomGenerator::global()->bounded(0, 100) < 70) {
        Obstaculo* nuevaPiedra = new Obstaculo(texturaPiedra, escena);
        listaObstaculos.push_back(nuevaPiedra);
    }
}

void MainWindow::animarMarea()
{
    if (juegoTerminado) return;

    tiempo += 0.02;
    double desplazamientoY = std::cos(tiempo) * 2.0;
    int posY = static_cast<int>(desplazamientoY) - 25;

    // Fondos
    for (size_t i = 0; i < listaFondos.size(); ++i) {
        listaFondos[i]->setPos(listaFondos[i]->x() - velocidadFondo, posY);
    }

    if (!listaFondos.empty() && listaFondos.front()->x() <= -ANCHO_FONDO) {
        QGraphicsPixmapItem* fondoSaliente = listaFondos.front();
        listaFondos.erase(listaFondos.begin());
        bool espejoSaliente = esEspejoFondo.front();
        esEspejoFondo.erase(esEspejoFondo.begin());

        QGraphicsPixmapItem* ultimo = listaFondos.back();
        double nuevaX = ultimo->x() + (ANCHO_FONDO - 1);
        fondoSaliente->setPos(nuevaX, posY);

        listaFondos.push_back(fondoSaliente);
        esEspejoFondo.push_back(espejoSaliente);
    }

    // Obstáculos
    for (auto it = listaObstaculos.begin(); it != listaObstaculos.end(); ) {
        Obstaculo* p = *it;
        if (p) p->actualizarPosicion(velocidadFondo);

        if (p && balsaItem->collidesWithItem(p)) {
            juegoTerminado = true;
            detenerTimers();
            mostrarGameOver("GAME OVER");
            return;
        }

        if (p && p->x() < -60) {
            escena->removeItem(p);
            delete p;
            it = listaObstaculos.erase(it);
        } else {
            ++it;
        }
    }

    // Discos
    for (auto it = listaDiscos.begin(); it != listaDiscos.end(); ) {
        Disco* d = *it;
        if (d == nullptr) {
            it = listaDiscos.erase(it);
            continue;
        }

        d->actualizarPosicion();

        // Si ya fue eliminado, lo sacamos de la lista
        if (d->scene() == nullptr) {
            it = listaDiscos.erase(it);
        } else {
            ++it;
        }
    }

    // Gaviotas
    for (auto it = listaGaviotas.begin(); it != listaGaviotas.end(); ) {
        Gaviota* g = *it;
        if (g && g->scene() != nullptr) {
            g->actualizarPosicion(velocidadFondo);
            ++it;
        } else {
            it = listaGaviotas.erase(it);
        }
    }
    double factor = velocidadFondo / 2.0;
    double fx = (-0.2 * factor) + (std::sin(tiempo) * 0.5);
    double fy = std::cos(tiempo) * 0.15;
    balsaItem->aplicarFisicasMarea(fx, fy);
}

void MainWindow::detenerTimers()
{
    if (timerMarea) timerMarea->stop();
    if (timerDificultad) timerDificultad->stop();
    if (timerSpawnObstaculos) timerSpawnObstaculos->stop();
    if (timerSpawnGaviotas) timerSpawnGaviotas->stop();
}

void MainWindow::mostrarGameOver(const QString& mensaje)
{
    textoGameOverItem = new QGraphicsTextItem(mensaje);
    QFont fuente("Arial", 40, QFont::Bold);
    textoGameOverItem->setFont(fuente);
    textoGameOverItem->setDefaultTextColor(Qt::red);
    textoGameOverItem->setTextWidth(800);
    textoGameOverItem->setPos(480, 320);
    escena->addItem(textoGameOverItem);
}

void MainWindow::keyPressEvent(QKeyEvent *evento)
{
    if (juegoTerminado) {
        if (evento->key() == Qt::Key_Return || evento->key() == Qt::Key_Enter) {
            reiniciarJuego();
        }
        return;
    }

    if (evento->key() == Qt::Key_Space) {
        balsaItem->lanzarDisco();
        return;
    }

    if (evento->key() == Qt::Key_Left || evento->key() == Qt::Key_A) balsaItem->moverIzquierda();
    else if (evento->key() == Qt::Key_Right || evento->key() == Qt::Key_D) balsaItem->moverDerecha();
    else if (evento->key() == Qt::Key_Up || evento->key() == Qt::Key_W) balsaItem->moverArriba();
    else if (evento->key() == Qt::Key_Down || evento->key() == Qt::Key_S) balsaItem->moverAbajo();
}

void MainWindow::keyReleaseEvent(QKeyEvent *evento)
{
    if (juegoTerminado) return;

    if (evento->key() == Qt::Key_Left || evento->key() == Qt::Key_A ||
        evento->key() == Qt::Key_Right || evento->key() == Qt::Key_D ||
        evento->key() == Qt::Key_Up || evento->key() == Qt::Key_W ||
        evento->key() == Qt::Key_Down || evento->key() == Qt::Key_S) {
        balsaItem->detenerMovimiento();
    }
}

void MainWindow::reiniciarJuego()
{
    for (auto p : listaObstaculos) { if(p){ escena->removeItem(p); delete p; }}
    listaObstaculos.clear();

    for (auto d : listaDiscos) { if(d){ escena->removeItem(d); delete d; }}
    listaDiscos.clear();

    for (auto g : listaGaviotas) { if(g){ escena->removeItem(g); delete g; }}
    listaGaviotas.clear();

    if (textoGameOverItem) {
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
    timerSpawnGaviotas->start(3800);
}

MainWindow::~MainWindow()
{
    reiniciarJuego();
    delete ui;
}

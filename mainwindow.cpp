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

    setWindowTitle("Ultimate Náufrago");
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

    textoVidasItem = new QGraphicsTextItem();
    textoVidasItem->setDefaultTextColor(Qt::white);
    textoVidasItem->setFont(QFont("Arial", 18, QFont::Bold));
    textoVidasItem->setPos(30, 20);
    escena->addItem(textoVidasItem);

    vista = new QGraphicsView(escena, this);
    vista->setGeometry(0, 0, 1600, 900);
    vista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vista->setRenderHint(QPainter::Antialiasing, true);
    vista->setRenderHint(QPainter::SmoothPixmapTransform, true);
    vista->setMouseTracking(true);
    vista->setInteractive(true);

    tiempo = 0.0;
    segundosTranscurridos = 0;
    velocidadFondo = 2.0;
    juegoTerminado = false;
    textoGameOverItem = nullptr;
    textoNivelCompletado = nullptr;
    nivelActual = 1;

    QPixmap pixmapOriginal(":/piedra.png");
    texturaPiedra = pixmapOriginal.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    timerMarea = new QTimer(this);
    connect(timerMarea, &QTimer::timeout, this, &MainWindow::animarMarea);

    timerDificultad = new QTimer(this);
    connect(timerDificultad, &QTimer::timeout, this, &MainWindow::aumentarDificultad);

    timerSpawnObstaculos = new QTimer(this);
    connect(timerSpawnObstaculos, &QTimer::timeout, this, &MainWindow::generarObstaculo);

    timerSpawnGaviotas = new QTimer(this);
    connect(timerSpawnGaviotas, &QTimer::timeout, this, &MainWindow::generarGaviota);

    mostrarMenuInicial();
}

void MainWindow::mostrarMenuInicial()
{
    menuActivo = true;

    menuTitulo = new QGraphicsTextItem("ULTIMATE NÁUFRAGO");
    menuTitulo->setDefaultTextColor(Qt::white);
    menuTitulo->setFont(QFont("Arial", 52, QFont::Bold));
    menuTitulo->setPos(420, 180);
    escena->addItem(menuTitulo);

    menuIniciar = new QGraphicsTextItem("▶  INICIAR JUEGO");
    menuIniciar->setDefaultTextColor(QColor(255, 215, 0));
    menuIniciar->setFont(QFont("Arial", 32, QFont::Bold));
    menuIniciar->setPos(560, 340);
    escena->addItem(menuIniciar);

    menuSalir = new QGraphicsTextItem("SALIR");
    menuSalir->setDefaultTextColor(Qt::white);
    menuSalir->setFont(QFont("Arial", 26));
    menuSalir->setPos(670, 420);
    escena->addItem(menuSalir);
}

void MainWindow::actualizarInterfazVidas()
{
    if (textoVidasItem && balsaItem) {
        textoVidasItem->setPlainText("VIDAS: " + QString::number(balsaItem->obtenerVidas()));
    }
}

void MainWindow::crearDisco(double posX, double posY, bool haciaDerecha)
{
    if (juegoTerminado) return;
    Disco* nuevoDisco = new Disco(escena, !haciaDerecha);
    nuevoDisco->setPos(posX, posY);
    escena->addItem(nuevoDisco);
    listaDiscos.push_back(nuevoDisco);
}

void MainWindow::generarGaviota()
{
    if (juegoTerminado) return;
    Gaviota* nueva = new Gaviota(escena, balsaItem);
    connect(nueva, &Gaviota::cacaLanzada, this, &MainWindow::agregarCaca);
    listaGaviotas.push_back(nueva);
}

void MainWindow::agregarCaca(Caca* caca)
{
    if (caca && escena) {
        listaCacas.push_back(caca);
    }
}

void MainWindow::generarObstaculo()
{
    if (juegoTerminado) return;
    if (QRandomGenerator::global()->bounded(0, 100) < 70) {
        Obstaculo* nuevaPiedra = new Obstaculo(texturaPiedra, escena);
        listaObstaculos.push_back(nuevaPiedra);
    }
}

void MainWindow::aumentarDificultad()
{
    if (juegoTerminado) return;
    segundosTranscurridos += 10;
    velocidadFondo += 1.2;
}

void MainWindow::spawnearJefe()
{
    if (juegoTerminado || jefeActual != nullptr) return;

    jefeActual = new Jefe(escena, balsaItem);
    connect(jefeActual, &Jefe::disparoAgua, this, &MainWindow::crearBalaAgua);
    qDebug() << "👹 ¡El JEFE ha aparecido!";
}

void MainWindow::spawnearJefeNivel2()
{
    ocultarMensajeNivelCompletado(); // Oculta el mensaje del nivel 1

    if (juegoTerminado || jefeActual != nullptr) return;

    jefeActual = new Jefe(escena, balsaItem);
    connect(jefeActual, &Jefe::disparoAgua, this, &MainWindow::crearBalaAguaNivel2);
    qDebug() << "👹 ¡Jefe del NIVEL 2 ha aparecido! (Dispara doble)";
}

void MainWindow::crearBalaAgua(qreal x, qreal y)
{
    if (juegoTerminado) return;

    BalaAgua* nuevaBala = new BalaAgua(escena, x, y);
    listaBalasAgua.push_back(nuevaBala);
}

void MainWindow::crearBalaAguaNivel2(qreal x, qreal y)
{
    if (juegoTerminado) return;

    BalaAgua* balaSeno = new BalaAgua(escena, x, y);
    balaSeno->setMovimientoSenoidal(true);
    listaBalasAgua.push_back(balaSeno);

    BalaAgua* balaCoseno = new BalaAgua(escena, x, y + 30);
    balaCoseno->setMovimientoSenoidal(false);
    listaBalasAgua.push_back(balaCoseno);
}

void MainWindow::ocultarMensajeNivelCompletado()
{
    if (textoNivelCompletado) {
        escena->removeItem(textoNivelCompletado);
        delete textoNivelCompletado;
        textoNivelCompletado = nullptr;
    }
}

void MainWindow::animarMarea()
{
    if (juegoTerminado || !balsaItem || !escena) return;

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
            balsaItem->recibirDanio();
            actualizarInterfazVidas();
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
        if (d && d->scene() != nullptr) {
            d->actualizarPosicion();
            ++it;
        } else {
            it = listaDiscos.erase(it);
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

    // Cacas
    for (auto it = listaCacas.begin(); it != listaCacas.end(); ) {
        Caca* c = *it;
        if (!c || c->scene() == nullptr) {
            it = listaCacas.erase(it);
            continue;
        }
        c->actualizar();

        bool eliminar = false;
        if (c->y() > 680) eliminar = true;

        if (balsaItem && c->collidesWithItem(balsaItem)) {
            balsaItem->recibirDanio();
            actualizarInterfazVidas();
            eliminar = true;
        }

        if (eliminar) {
            escena->removeItem(c);
            delete c;
            it = listaCacas.erase(it);
        } else {
            ++it;
        }
    }

    // Colisión Discos vs Jefe + Muerte del Jefe
    if (jefeActual) {
        for (auto it = listaDiscos.begin(); it != listaDiscos.end(); ) {
            Disco* d = *it;
            if (d && d->collidesWithItem(jefeActual)) {
                jefeActual->recibirDanio(1);
                escena->removeItem(d);
                delete d;
                it = listaDiscos.erase(it);
            } else {
                ++it;
            }
        }

        if (jefeActual && jefeActual->obtenerVida() <= 0) {
            escena->removeItem(jefeActual);
            delete jefeActual;
            jefeActual = nullptr;

            qDebug() << "💀 El JEFE ha sido derrotado";

            if (nivelActual == 1) {
                nivelActual = 2;
                velocidadFondo += 1.8;

                // Mensaje de nivel completado
                if (textoNivelCompletado) {
                    escena->removeItem(textoNivelCompletado);
                    delete textoNivelCompletado;
                }

                textoNivelCompletado = new QGraphicsTextItem("¡NIVEL 1 COMPLETADO!\nVelocidad aumentada");
                QFont fuente("Arial", 32, QFont::Bold);
                textoNivelCompletado->setFont(fuente);
                textoNivelCompletado->setDefaultTextColor(Qt::yellow);
                textoNivelCompletado->setTextWidth(700);
                textoNivelCompletado->setPos(450, 280);
                escena->addItem(textoNivelCompletado);

                // Ocultar mensaje automáticamente después de 5 segundos
                QTimer::singleShot(5000, this, &MainWindow::ocultarMensajeNivelCompletado);

                timerNivel2Jefe = new QTimer(this);
                timerNivel2Jefe->setSingleShot(true);
                connect(timerNivel2Jefe, &QTimer::timeout, this, &MainWindow::spawnearJefeNivel2);
                timerNivel2Jefe->start(60000);
            }
        }
    }

    // Balas de agua
    for (auto it = listaBalasAgua.begin(); it != listaBalasAgua.end(); ) {
        BalaAgua* bala = *it;

        if (!bala || bala->scene() == nullptr) {
            it = listaBalasAgua.erase(it);
            continue;
        }

        bala->actualizar();

        bool eliminar = false;

        if (bala->x() < -70 || bala->y() > 900) {
            eliminar = true;
        }

        if (balsaItem && bala->collidesWithItem(balsaItem)) {
            balsaItem->recibirDanio();
            actualizarInterfazVidas();
            eliminar = true;
        }

        if (eliminar) {
            escena->removeItem(bala);
            delete bala;
            it = listaBalasAgua.erase(it);
        } else {
            ++it;
        }
    }

    // Game Over
    if (balsaItem->obtenerVidas() <= 0 && !juegoTerminado) {
        juegoTerminado = true;

        if (timerMarea) timerMarea->stop();
        if (timerDificultad) timerDificultad->stop();
        if (timerSpawnObstaculos) timerSpawnObstaculos->stop();
        if (timerSpawnGaviotas) timerSpawnGaviotas->stop();
        if (timerSpawnJefe) timerSpawnJefe->stop();
        if (timerNivel2Jefe) timerNivel2Jefe->stop();

        textoGameOverItem = new QGraphicsTextItem("GAME OVER\nPresiona ENTER para reiniciar");
        QFont fuente("Arial", 36, QFont::Bold);
        textoGameOverItem->setFont(fuente);
        textoGameOverItem->setDefaultTextColor(Qt::red);
        textoGameOverItem->setTextWidth(900);
        textoGameOverItem->setPos(400, 300);
        escena->addItem(textoGameOverItem);
        return;
    }

    double factor = velocidadFondo / 2.0;
    double fx = (-0.2 * factor) + (std::sin(tiempo) * 0.5);
    double fy = std::cos(tiempo) * 0.15;
    balsaItem->aplicarFisicasMarea(fx, fy);
}

void MainWindow::mousePressEvent(QMouseEvent *evento)
{
    if (!menuActivo) {
        QMainWindow::mousePressEvent(evento);
        return;
    }

    QPointF scenePos = vista->mapToScene(evento->pos());

    if (menuIniciar && menuIniciar->sceneBoundingRect().contains(scenePos)) {
        if (menuTitulo) escena->removeItem(menuTitulo);
        if (menuIniciar) escena->removeItem(menuIniciar);
        if (menuSalir) escena->removeItem(menuSalir);

        delete menuTitulo;
        delete menuIniciar;
        delete menuSalir;

        menuTitulo = nullptr;
        menuIniciar = nullptr;
        menuSalir = nullptr;
        menuActivo = false;

        timerMarea->start(16);
        timerDificultad->start(10000);
        timerSpawnObstaculos->start(2500);
        timerSpawnGaviotas->start(15000);

        actualizarInterfazVidas();

        vista->setFocus();
        this->grabKeyboard();

        timerSpawnJefe = new QTimer(this);
        timerSpawnJefe->setSingleShot(true);
        connect(timerSpawnJefe, &QTimer::timeout, this, &MainWindow::spawnearJefe);
        timerSpawnJefe->start(30000);
    }
    else if (menuSalir && menuSalir->sceneBoundingRect().contains(scenePos)) {
        close();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *evento)
{
    if (juegoTerminado) {
        if (evento->key() == Qt::Key_Return || evento->key() == Qt::Key_Enter) {
            reiniciarJuego();
        }
        return;
    }

    if (evento->key() == Qt::Key_J && jefeActual == nullptr && !juegoTerminado) {
        if (nivelActual == 1)
            spawnearJefe();
        else
            spawnearJefeNivel2();
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
    for (auto p : listaObstaculos) { if (p && p->scene()) escena->removeItem(p); delete p; }
    listaObstaculos.clear();

    for (auto d : listaDiscos) { if (d && d->scene()) escena->removeItem(d); delete d; }
    listaDiscos.clear();

    for (auto g : listaGaviotas) { if (g && g->scene()) escena->removeItem(g); delete g; }
    listaGaviotas.clear();

    for (auto c : listaCacas) { if (c && c->scene()) escena->removeItem(c); delete c; }
    listaCacas.clear();

    for (auto b : listaBalasAgua) { if (b && b->scene()) escena->removeItem(b); delete b; }
    listaBalasAgua.clear();

    if (jefeActual) {
        if (jefeActual->scene()) escena->removeItem(jefeActual);
        delete jefeActual;
        jefeActual = nullptr;
    }

    if (textoGameOverItem) {
        if (textoGameOverItem->scene()) escena->removeItem(textoGameOverItem);
        delete textoGameOverItem;
        textoGameOverItem = nullptr;
    }

    if (textoNivelCompletado) {
        if (textoNivelCompletado->scene()) escena->removeItem(textoNivelCompletado);
        delete textoNivelCompletado;
        textoNivelCompletado = nullptr;
    }

    if (timerNivel2Jefe) {
        timerNivel2Jefe->stop();
        timerNivel2Jefe->deleteLater();
        timerNivel2Jefe = nullptr;
    }

    if (balsaItem) {
        balsaItem->setPos(100, 450);
        balsaItem->restablecerVidas();
    }

    actualizarInterfazVidas();

    tiempo = 0.0;
    segundosTranscurridos = 0;
    velocidadFondo = 2.0;
    juegoTerminado = false;
    nivelActual = 1;

    timerMarea->start(16);
    timerDificultad->start(10000);
    timerSpawnObstaculos->start(2500);
    timerSpawnGaviotas->start(15000);
}

MainWindow::~MainWindow()
{
    reiniciarJuego();
    delete ui;
}

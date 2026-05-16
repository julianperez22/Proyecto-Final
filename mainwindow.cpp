#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QIcon>   // Necesario para el logo
#include <QDebug>  // Necesario para imprimir en consola
#include <QFile>   // 👈 ¡FALTABA ESTA LÍNEA! Necesario para verificar si el archivo existe

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Ultimate Náufrago");
    resize(800, 600);

    /* Prueba de control: Comprobamos si Qt realmente ve el archivo en los recursos
    bool existe = QFile::exists(":/logo.png");
    qDebug() << "---------------------------------------------";
    qDebug() << "¿EL LOGO EXISTE EN LOS RECURSOS?: " << (existe ? "SÍ (TRUE)" : "NO (FALSE)");
    qDebug() << "---------------------------------------------";

    setWindowIcon(QIcon(":/logo.png"));*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

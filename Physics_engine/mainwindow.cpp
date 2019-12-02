#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "world.h"
#include "engine.h"
#include "object.h"
#include <QTimer>
#include <QDebug>

//#include <QLCDNumber>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ui basic settings
    ui->gravityValue->setRange(0, 100);
    ui->gravityValue->setValue(9.8);
    ui->frictionValue->setRange(0, 1);
    ui->velocityValueX->setRange(-100,100);
    ui->velocityValueY->setRange(-100,100);
    ui->restitutionValue->setRange(0,1);
    ui->restitutionValue->setValue(1.0);

    world = new World(&engine, this);
    ui->verticalLayout->addWidget(world, 0, 0);

// QTimer는 Object의 event 를 triger 한다.
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, world, &World::animate); // todo: ui랑 연결이 안되어 있음..
    timer->start(1); // 계산하는 간격을 의미하는 듯. fps와 비슷.
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_applyButton_clicked()
{
    double gravity = ui->gravityValue->value();
    engine.setGravity(gravity);
    double friction = ui->frictionValue->value();
    engine.setFriction(friction);
}

void MainWindow::on_defaultButton_clicked()
{
    ui->gravityValue->setValue(9.8);
    engine.setGravity(9.8);
    ui->frictionValue->setValue(0);
    engine.setFriction(0);
}


void MainWindow::on_setButton_clicked()
{
    QString shape = ui->shapeComboBox->currentText();
    double mass = ui->massValue->value();
    double sizeX = ui->sizeValueX->value();
    double sizeY = ui->sizeValueY->value();
    double velocityX = ui->velocityValueX->value() ;
    double velocityY = ui->velocityValueY->value() ;
    double restitution = ui->restitutionValue->value();
    if (shape == "Circle"){
        Circle circle = Circle(&engine, mass, sizeX, sizeY, velocityX, velocityY, restitution);
        world->createCircleEvent(circle);
    } else if (shape == "Rectangle"){
        Rectangle rectangle = Rectangle(&engine, mass, sizeX, sizeY, velocityX, velocityY, restitution);
        world->createRectEvent(rectangle);
    }

    //    painter.drawEllipse(QPointF(x,y), radius, radius);
}

void MainWindow::on_startButton_clicked()
{
    double dt=0.1;
    engine.setDelta(dt);
}


void MainWindow::on_stopButton_clicked()
{
    engine.setDelta(0);
}

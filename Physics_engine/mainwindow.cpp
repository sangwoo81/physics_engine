#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "world.h"

#include <QTimer>
//#include <QLCDNumber>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->gravityValue->setRange(0, 20);
    ui->gravityValue->setSpecialValueText(tr("9.8 (Earth)"));
    ui->frictionValue->setRange(0, 1);
    connect(ui->applyButton, SIGNAL(clicked()),
            this, SLOT(on_applyButton_clicked()));
    connect(ui->defaultButton, SIGNAL(clicked()),
            this, SLOT(on_defaultButton_clicked()));

    World *world = new World(&engine, this);
    ui->verticalLayout->addWidget(world, 0, 0);
//    bool enable=true;
//    world->setMouseTracking(enable);

//    connect(world, &World::positionXChanged,
//            ui->positionX,QOverload<int>::of(&QLCDNumber::display) );

//    connect(world, &World::positionYChanged,
//            ui->positionY, QOverload<int>::of(&QLCDNumber::display));

// QTimer는 Object의 event 를 triger 한다.
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, world, &World::animate); // todo: ui랑 연결이 안되어 있음..
    timer->start(50); // 계산하는 간격을 의미하는 듯. fps와 비슷.
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_applyButton_clicked()
{
    double gravity = ui->gravityValue->value();
    engine.Set_gravity(gravity);
    double friction = ui->frictionValue->value();
    engine.Set_friction(friction);
}

void MainWindow::on_defaultButton_clicked()
{
    ui->gravityValue->setValue(9.8);
    engine.Set_gravity(9.8);
    ui->frictionValue->setValue(0);
    engine.Set_friction(0);
}


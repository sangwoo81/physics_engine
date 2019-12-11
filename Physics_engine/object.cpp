#include "object.h"
#include "collision.h"
#include "engine.h"

#include <QDebug>
#include <cmath>
#include <algorithm>


Object::Object(Engine *engine, double mass, QPointF position, QPointF velocity, double restitution, bool isMovingY)://, bool isFixed):
    engine(engine), position(position), velocity(velocity), mass(mass), restitution(restitution), isMovingY(isMovingY)//, isFixed(isFixed)
{
    setMassInversion();
    qDebug() << "Object is created.";
}

void Object::positionUpdate(){
    double g=engine->getGravity();
    double delta=engine->getDelta();
    position.rx()+=velocity.rx()*delta;
    if(isMovingY){
        position.ry()+=velocity.ry()*delta;
        velocity.ry()+=g*delta;
    }
//    qDebug() << position.rx()<< ", "<< position.ry();
}

Rectangle::Rectangle(Engine *engine, double mass, QPointF size, QPointF position, QPointF velocity, double restitution, bool isMovingY):
   Object(engine, mass, position, velocity, restitution, isMovingY), width(size.x()), height(size.y())//, isMovingY(isMoving)
{}

void Rectangle::bounce(){
    bool collision =rectangleVsGround(*this);
    if (collision) {
        qDebug() << "rec bounce!";
        velocity.setY(-restitution*std::sqrt(std::abs(velocity.ry()*velocity.ry()-5))*0.98);
    }
}

Circle::Circle(Engine *engine, double mass, double radius, QPointF position, QPointF velocity, double restitution, bool isMovingY):
    Object(engine, mass, position, velocity, restitution, isMovingY), radius(radius)//, isMovingY(isMoving)
{}

void Circle::bounce(){
    bool collision =circleVsGround(*this);
    if (collision) {
        qDebug() << "cir bounce!";
        velocity.setY(-restitution*std::sqrt(std::abs(velocity.ry()*velocity.ry()-5))*0.98); // 0.98 for stability
    }
}

void Manifold::updateCircleVsCircle(){
    bool collision = circleVsCircle(this);
    if (collision) {
        positionCorrection(this);
        resolveCollision(this);
    }
}

void Manifold::updateRectangleVsRectangle(){
    bool collision = rectangleVsRectangle(this);
    if (collision) {
        positionCorrection(this);
        resolveCollision(this);
    }
}

void Manifold::updateRectangleVsCircle(){
    bool collision = rectangleVsCircle(this);
    if (collision) {
        positionCorrection(this);
        resolveCollision(this);
    }
}

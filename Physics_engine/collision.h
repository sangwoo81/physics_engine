#ifndef COLLISION_H
#define COLLISION_H

#include "object.h"

#include <QDebug>
#include <cmath>
#include <algorithm>
#include <QPointF>


// simple distance function: position to position
double Distance(const QPointF &a, const QPointF &b = {0 , 0}){
    return std::sqrt(std::pow((a.x()-b.x()),2) + std::pow((a.y()-b.y()),2));
}

//// collision detectrion function: rectangle vs rectangle
//bool Rec_vs_rec(const Rectangle &rec1, const Rectangle &rec2){
//    // if objects impulsed, it return true. otherwise return false
//    if(rec1.Right_bottom().x() < rec2.Left_top().x() or rec1.Left_top().x() > rec2.Right_bottom().x()) return false;
//    if(rec1.Right_bottom().y() > rec2.Left_top().y() or rec1.Left_top().y() < rec2.Right_bottom().y()) return false;
//    return true;
//}

//bool Cir_vs_cir(const Circle &cir1, const Circle &cir2){
//    if (Distance(cir1.position, cir2.position) > cir1.radius + cir2.radius) return false;
//    return true;
//}

double Clamp(double min, double max, double value){
    if (value<min) value=min;
    if (value>max) value=max;
    return value;
}

//QPointF velocityDamper(QPointF velocity){
//    QPointF damped;
//    double focus = 5;
//    double rate = 1.02;
//    const int signX = (velocity.x() > 0) ? 1:-1;
//    const int signY = (velocity.y() > 0) ? 1:-1;
//    damped.setX(velocity.rx());
//    damped.setY(signY*std::sqrt(std::abs(velocity.ry()*velocity.ry()+focus))*rate);
//    return damped;
//}

bool circleVsCircle( Manifold *m )
{
  // Setup a couple pointers to each object
    Object *A = m->A;
    Object *B = m->B;
    double radiusA=A->getRadius(); //todo: could be problem
    double radiusB=B->getRadius();
  // Vector from A to B
    QPointF positionRelative = B->position - A->position;
    double distance = Distance(positionRelative);

    double radiusTotal = radiusA+radiusB;
//  r *= r;

    if(distance > radiusTotal)
        return false;

  // Circles have collided, now compute manifold
     // perform actual sqrt

  // If distance between circles is not zero
    if(distance != 0){
    // Distance is difference between radius and distance
        m->penetration = radiusTotal - distance;

    // Utilize our d since we performed sqrt on it already within Length( )
    // Points from A to B, and is a unit vector
        m->normal = positionRelative / distance;
        return true;
    }

  // Circles are on same position
    else{
    // Choose random (but consistent) values
        m->penetration = radiusA;
        m->normal = QPointF(1,0);
        return true;
    }
}

bool rectangleVsRectangle( Manifold *m )
{
  // Setup a couple pointers to each object
    Object *A = m->A;
    Object *B = m->B;

  // Vector from A to B
    QPointF positionRelative = B->position - A->position;
    QPointF velocityRelative = B->velocity - A->velocity;


  // Calculate half extents along x axis for each object
    double widthA=A->getWidth();
    double widthB=B->getWidth();

  // Calculate overlap on x axis
    double overlapX = 0.5*(widthA+widthB) - abs(positionRelative.x());

  // SAT test on x axis
    if(overlapX > 0){
    // Calculate half extents along x axis for each object
        double heightA=A->getHeight();
        double heightB=B->getHeight();

        double overlapY = 0.5*(heightA+heightB) - abs(positionRelative.y());

        // SAT test on y axis
        if(overlapY > 0) {
          // Find out which axis is axis of least penetration
            if(overlapX < overlapY) {
            // Point towards B knowing that n points from A to B
//                if(positionRelative.x() < 0) { m->normal = QPointF( -1, 0 );}
                if(velocityRelative.x() > 0) { m->normal = QPointF( -1, 0 );}
                else { m->normal = QPointF( 1, 0 );}
            m->penetration = overlapX;
            return true;
            }
            else {
            // Point toward B knowing that n points from A to B
//                if(positionRelative.y() < 0) {m->normal = QPointF( 0, -1 );}
                if(velocityRelative.y() > 0) {m->normal = QPointF( 0, -1 );}
                else { m->normal = QPointF( 0, 1 );}
            m->penetration = overlapY;
            return true;
            }
        }
    }
    return false;
}

bool rectangleVsCircle(Manifold *m){
    // Setup a couple pointers to each object
    Object *A = m->A;
    Object *B = m->B;

    // Vector from A to B
    QPointF positionRelative = B->position - A->position;

    const int signX = (positionRelative.x() > 0) ? 1:-1;
    const int signY = (positionRelative.y() > 0) ? 1:-1;

    QPointF closest = positionRelative;

    const double widthA=A->getWidth();
    const double heightA=A->getHeight();

    closest.rx() = Clamp(-0.5*widthA, 0.5*widthA, closest.x());
    closest.ry() = Clamp(-0.5*heightA, 0.5*heightA, closest.y());

    bool inside = false;

    if (positionRelative==closest){
        inside = true;

    // Find closest axis
        if(abs( positionRelative.x() ) > abs( positionRelative.y() )) closest.rx() = signX*0.5*widthA;
        else closest.ry() = signY*0.5*heightA;
    }

    QPointF normal = positionRelative - closest;
    double distance = Distance(normal);

    const double radius=B->getRadius();

    // Early out of the radius is shorter than distance to closest point and
    // Circle not inside the AABB
    if(distance > radius && !inside) return false;

    // Avoided sqrt until we needed

    // Collision normal needs to be flipped to point outside if circle was
    // inside the AABB
    if(inside) {
        m->normal = -normal/distance;
        m->penetration = radius + distance;
    }
    else{
        m->normal = normal/distance;
        m->penetration = radius - distance;
    }
    return true;
}

//bool rectangleVsGround(Rectangle &rec){
//    qreal ground=250;
//    if(rec.Right_bottom().y() > ground) {
//        rec.position.setY( ground - 0.5*rec.height);
//        if (rec.velocity.ry() < 5) {
//            rec.stopY();
//            rec.velocity.setY(0);
//        }
//        return true;

//    }
//    return false;
//}

//bool circleVsGround(Circle &cir){
//    qreal ground=250;
//    if((cir.position.y()+cir.radius) > ground) {
//        cir.position.setY( ground - cir.radius);
//        if(cir.velocity.ry() < 5){
//            cir.stopY();
//            cir.velocity.setY(0);
//        }
//        return true;
//    }
//    return false;
//}

void positionCorrection(Manifold *m){
    const double percent = 1.01; // usually 20% to 80%
    const double  slop = 0.01; // usually 0.01 to 0.1
    Object *A=m->A;
    Object *B=m->B;

    if (A->massInv + B->massInv != 0 ){
    QPointF correction = std::max(m->penetration - slop, 0.0 ) / (A->massInv + B->massInv) * percent * m->normal;
    A->position -= A->massInv * correction;
    B->position += B->massInv * correction;
    }
}

void resolveCollision(Manifold *m){
    Object *A=m->A;
    Object *B=m->B;

    // Calculate relative velocity
    QPointF velocityRelative = B->velocity - A->velocity;

    // Calculate relative velocity in terms of the normal direction
    double velocityDotNormal = QPointF::dotProduct( velocityRelative, m->normal );

    // Do not resolve if velocities are separating
    if(velocityDotNormal > 0) return;

    // Calculate restitution
    double e = std::min(std::min( A->restitution, B->restitution),0.98);

    // Calculate impulse scalar
    if (A->massInv + B->massInv != 0 ){
        double j = -(1 + e) * velocityDotNormal / (A->massInv + B->massInv);
        double declineFactor = 8/(A->massInv + B->massInv);
        if (A->massInv==0 | B->massInv ==0){
            if (j>declineFactor){
                declineFactor *= declineFactor;
                j = std::sqrt(j*j - declineFactor);
            } else {
                j=0.5*j;
            }
        }

        // Apply impulse
        QPointF impulse = j * m->normal;

        A->velocity -= A->massInv * impulse;
        B->velocity += B->massInv * impulse;
    }
}


#endif // COLLISION_H

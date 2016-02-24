//
//  Point.cpp
//  Potree
//
//  Created by QuocDinh on 20/01/2016.
//
//

#include "Point.h"
#include "Vector3.h"

namespace Potree{

Vector3<double> Point::getPosition()
{
    return position;
}
    
void Point::setPosition(const Vector3<double> &p)
{
    position.x = p.x;
    position.y = p.y;
    position.z = p.z;
}

}
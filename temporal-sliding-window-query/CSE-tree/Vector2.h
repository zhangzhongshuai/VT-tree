//
// Created by zcc on 2021/6/22.
//

#ifndef QUADTRANS_VECTOR2_H
#define QUADTRANS_VECTOR2_H
#pragma once
class Vector2{
public:
    double x;
    double y;
    Vector2(int x=0,int y=0){
        this->x=x;
        this->y=y;
    }
    Vector2& operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vector2& operator/=(double t)
    {
        x /= t;
        y /= t;
        return *this;
    }

};
Vector2 operator/(Vector2 vec, double t)
{
    vec /= t;
    return vec;
}
Vector2 operator+(Vector2 lhs, const Vector2& rhs) noexcept
{
    lhs += rhs;
    return lhs;
}
#endif //QUADTRANS_VECTOR2_H

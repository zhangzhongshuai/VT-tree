//
// Created by zcc on 2021/6/24.
//

#ifndef QUADTRANS_POINT_H
#define QUADTRANS_POINT_H
class Point{
public:
    double x, y;
    Point(double x=0, double y=0):x(x),y(y) {}
};
typedef Point Vector;

Vector operator - (const Point& A, const Point& B) {
    return Vector(A.x-B.x, A.y-B.y);
}

double Cross(const Vector& A, const Vector& B) {
    return A.x*B.y - A.y*B.x;
}

double Dot(const Vector& A, const Vector& B) {
    return A.x*B.x + A.y*B.y;
}

bool operator < (const Point& p1, const Point& p2) {
    return p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y);
}

bool operator == (const Point& p1, const Point& p2) {
    return p1.x == p2.x && p1.y == p2.y;
}
#endif //QUADTRANS_POINT_H

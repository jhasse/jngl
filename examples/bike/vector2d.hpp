#pragma once

class Vector2d
{
public:
    Vector2d(double x, double y);
    Vector2d(double v);
    Vector2d();
    double X() const;
    double Y() const;
    void Set(double x, double y);
	void SetX(double x);
	void SetY(double y);
    Vector2d& operator/=(const double v);
    Vector2d& operator*=(const double v);
    Vector2d& operator-=(const Vector2d& rhs);
    Vector2d& operator-=(const double v);
    Vector2d& operator+=(const double v);
    Vector2d& operator+=(const Vector2d& rhs);
    Vector2d& operator++();
    Vector2d& operator--();
    double Length() const;
    double LengthSq() const;
    double Sum() const;
    Vector2d& Normalize();
private:
    double x_, y_;
};

Vector2d operator*(const Vector2d& lhs, const double v);
Vector2d operator/(const Vector2d& lhs, const double v);
Vector2d operator*(const double v, const Vector2d& rhs);
Vector2d operator/(const double v, const Vector2d& rhs);
double operator*(const Vector2d& lhs, const Vector2d& rhs);
Vector2d operator-(const Vector2d& lhs, const Vector2d& rhs);
Vector2d operator+(const Vector2d& lhs, const Vector2d& rhs);

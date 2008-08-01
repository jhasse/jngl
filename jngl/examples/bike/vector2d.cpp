#include "vector2d.hpp"
#include <cmath>

double operator*(const Vector2d& lhs, const Vector2d& rhs)
{
    return lhs.X() * rhs.X() +lhs.Y() * rhs.Y();
}

Vector2d operator*(const Vector2d& lhs, const double v)
{
    return Vector2d(lhs.X() * v, lhs.Y() * v);
}

Vector2d operator/(const Vector2d& lhs, const double v)
{
    return Vector2d(lhs.X() / v, lhs.Y() / v);
}

Vector2d operator*(const double v, const Vector2d& rhs)
{
    return Vector2d(rhs.X() * v, rhs.Y() * v);
}

Vector2d operator/(const double v, const Vector2d& rhs)
{
    return Vector2d(rhs.X() / v, rhs.Y() / v);
}

Vector2d operator-(const Vector2d& lhs, const Vector2d& rhs)
{
    return Vector2d(lhs.X() - rhs.X(), lhs.Y() - rhs.Y());
}

Vector2d operator+(const Vector2d& lhs, const Vector2d& rhs)
{
    return Vector2d(lhs.X() + rhs.X(), lhs.Y() + rhs.Y());
}

Vector2d& Vector2d::Normalize()
{
    const double temp = Length();
    x_ /= temp;
    y_ /= temp;
    return *this;
}

double Vector2d::Sum() const
{
    return x_ + y_;
}

Vector2d::Vector2d() : x_(0), y_(0)
{}

Vector2d::Vector2d(double v) : x_(v), y_(v)
{}

Vector2d::Vector2d(double x, double y) : x_(x), y_(y)
{}

double Vector2d::LengthSq() const
{
    return x_*x_ + y_*y_;
}

double Vector2d::X() const
{
    return x_;
}

double Vector2d::Y() const
{
    return y_;
}

void Vector2d::SetX(const double x)
{
    x_ = x;
}

void Vector2d::SetY(const double y)
{
    y_ = y;
}

void Vector2d::Set(const double x, const double y)
{
    x_ = x;
    y_ = y;
}

Vector2d& Vector2d::operator/=(const double v)
{
    x_ /= v;
    y_ /= v;
    return *this;
}

Vector2d& Vector2d::operator*=(const double v)
{
    x_ *= v;
    y_ *= v;
    return *this;
}

Vector2d& Vector2d::operator-=(const Vector2d& rhs)
{
    x_ -= rhs.X();
    y_ -= rhs.Y();
    return *this;
}

Vector2d& Vector2d::operator-=(const double v)
{
    x_ -= v;
    y_ -= v;
    return *this;
}

Vector2d& Vector2d::operator+=(const double v)
{
    x_ += v;
    y_ += v;
    return *this;
}

Vector2d& Vector2d::operator+=(const Vector2d& rhs)
{
    x_ += rhs.X();
    y_ += rhs.Y();
    return *this;
}

Vector2d& Vector2d::operator++()
{
    ++x_;
    ++y_;
    return *this;
}

Vector2d& Vector2d::operator--()
{
    --x_;
    --y_;
    return *this;
}

double Vector2d::Length() const
{
    return sqrt(x_*x_ + y_*y_);
}

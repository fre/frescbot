#include "Vector2D.h"

#include <cmath>

Vector2D::Vector2D(double _x, double _y)
  : x(_x),
    y(_y)
{
}

Vector2D::Vector2D(BWAPI::Position p)
  : x(p.x()),
    y(p.y())
{
}

bool
Vector2D::isNull() const
{
	return x == 0 && y == 0;
}

Vector2D::operator BWAPI::Position() const
{
	return BWAPI::Position((int) x, (int) y);
}

Vector2D
Vector2D::operator+(const Vector2D& v) const
{
	return Vector2D(x + v.x, y + v.y);
}

Vector2D
Vector2D::operator-(const Vector2D& v) const
{
	return Vector2D(x - v.x, y - v.y);
}

Vector2D
Vector2D::operator*(double d) const
{
	return Vector2D(x * d, y * d);
}

Vector2D
Vector2D::operator/(double d) const
{
	return Vector2D(x / d, y / d);
}

Vector2D&
Vector2D::operator+=(const Vector2D& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

Vector2D&
Vector2D::operator-=(const Vector2D& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

Vector2D&
Vector2D::operator*=(double d)
{
	x *= d;
	y *= d;
	return *this;
}

Vector2D&
Vector2D::operator/=(double d)
{
	x /= d;
	y /= d;
	return *this;
}

bool
Vector2D::operator == (const Vector2D& v) const
{
	return x == v.x && y == v.y;
}

bool
Vector2D::operator != (const Vector2D& v) const
{
	return x != v.x || y != v.y;
}

double
Vector2D::getLength() const
{
	return sqrt(x * x + y * y);
}

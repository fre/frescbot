#pragma once

#include <BWAPI.h>

struct Vector2D
{
	Vector2D(double x = 0, double y = 0);
	Vector2D(BWAPI::Position p);

	bool isNull() const;
	operator BWAPI::Position() const;

	Vector2D operator+(const Vector2D& v) const;
  Vector2D operator-(const Vector2D& v) const;
  Vector2D operator*(double d) const;
  Vector2D operator/(double d) const;
  Vector2D& operator+=(const Vector2D& v);
  Vector2D& operator-=(const Vector2D& v);
  Vector2D& operator*=(double d);
  Vector2D& operator/=(double d);
	bool operator == (const Vector2D& v) const;
  bool operator != (const Vector2D& v) const;

	double getLength() const;

	double x;
	double y;
};

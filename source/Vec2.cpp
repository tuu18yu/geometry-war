#include "Vec2.h"
#include <cmath>

Vec2::Vec2()
	: x(0), y(0)
{
}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{
}

bool Vec2::operator == (const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2& rhs) const
{
	return (x != rhs.x || y != rhs.y);
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return (Vec2(x + rhs.x, y + rhs.y));
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return (Vec2(x - rhs.x, y - rhs.y));
}

Vec2 Vec2::operator / (const float val) const
{
	return (Vec2(x / val, y / val));
}

Vec2 Vec2::operator * (const float val) const
{
	return Vec2(x * val, y * val);
}

void Vec2::operator += (const Vec2& rhs)
{
	x = x + rhs.x;
	y = y + rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	x = x - rhs.x;
	y = y - rhs.y;
}

void Vec2::operator *= (const float val)
{
	x = x * val;
	y = y * val;
}

void Vec2::operator /= (const float val)
{
	x = x / val;
	y = y / val;
}


float Vec2::getLength() const
{
	return sqrt(pow(x, 2.0) + pow(y, 2.0));
}

void Vec2::normalize()
{
	float L = getLength();
	x = x / L;
	y = y / L;
}

std::string Vec2::toString()
{
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}

const std::string Vec2::toString() const
{
	return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}
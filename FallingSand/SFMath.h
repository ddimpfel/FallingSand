/*
* Utility class to expand sf vector functionality and ease of use
* @author Dominick Dimpfel
* @date 02/02/2024
*/
#ifndef SFMATH_H
#define SFMATH_H

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <iomanip>
#include <iostream>

using Vec2f = sf::Vector2f;
using Vec2i = sf::Vector2i;

#define RAD2DEG(x)			((x) * 57.295754f)
#define DEG2RAD(x)			((x) * 0.0174533f)

class SFMath
{
	static constexpr float pi			= 3.141592653f;
	static constexpr float pi_half		= 1.570796326f;
	static constexpr float pi_fourth	= 0.785398163f;
	static constexpr float pi_sixth		= 0.523598775f;
	static constexpr float epsilon		= 0.000001f;

	static void zero(Vec2f& v) { v.x = 0; v.y = 0; }

	static float length(const Vec2f& v) 
	{ 
		return std::sqrt(v.x * v.x + v.y * v.y);
	}

	static float lengthSqrd(const Vec2f& v)
	{ 
		return v.x * v.x + v.y * v.y; 
	}

	static float distance(const Vec2f& vl, const Vec2f& vr)
	{ 
		return std::sqrt(vl.x * vr.x + vl.y * vr.y);
	}

	static float distanceSqrd(const Vec2f& vl, const Vec2f& vr)
	{ 
		return vl.x * vr.x + vl.y * vr.y; 
	}

	static Vec2f normalize(const Vec2f& v)
	{ 
		float l = length(v);
		return l == 0 ? Vec2f(0, 0) : Vec2f(v.x / l, v.y / l);
	}

	/*
	* Returns the magnitude of directionality between two vectors.
	* Large return means they are collinear.
	* 0 return means they are perpendicular.
	*/
	static float dot(const Vec2f& vl, const Vec2f& vr)
	{
		return vl.x * vr.x + vl.y * vr.y; 
	}

	/*
	* Cross product of two vectors. The result is a scalar
	* representing the signed area of the parallelogram created by them.
	* The sign represents rotation; if positive, vr is ccw from vl.
	* @return float signed area of parallelogram
	*/
	static float cross(const Vec2f& vl, const Vec2f& vr)
	{
		return vl.x * vr.y - vl.y * vr.x;
	}

	/*
	* Determine the direction three points of a line turn in.
	* If the result is 0, the points are collinear; if it is positive, the
	* three points constitute a "left turn" or, otherwise a "right turn"
	* (X2 - X1)(Y3 - Y1) - (Y2 - Y1)(X3 - X1)
	*/
	static float cross3(const Vec2f& p1, const Vec2f& p2, const Vec2f& p3)
	{
		return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
	}

	/*
	* Cross product between a vector and a scalar. The result is a 
	* perpendicular vector rotated 90 degrees cw (if scalar is positive).
	*/
	static Vec2f crossScl(const Vec2f& v, float scalar)
	{
		return { scalar * v.y, -scalar * v.x };
	}

	/*
	* Angle between left vector and right vector.
	* @return angle between -1 and 1, -1 is 180 degrees away from vl
	*/
	static float angle(const Vec2f& vl, const Vec2f& vr)
	{
		float leftLSq = lengthSqrd(vl);
		float rightLSq = lengthSqrd(vr);
		return rightLSq == 0 || leftLSq == 0 ? 0 : dot(vl, vr) / std::sqrt(leftLSq * rightLSq);
	}

	/*
	* Project a vector onto another.
	* @return new Vec2f
	*/
	static Vec2f project(const Vec2f& proj, const Vec2f& onto)
	{
		float d = dot(proj, onto);
		float ontoLSq = lengthSqrd(onto);
		return onto * (d / ontoLSq);
	}

	/*
	* Find the perpendicular vector from a point projected onto another
	* @return new Vec2f
	*/
	static Vec2f perp(const Vec2f& point, const Vec2f& onto)
	{
		return point - project(point, onto);
	}

	/*
	* Reflect a vector v over a normal axis.
	* @return new Vec2f
	*/
	static Vec2f reflect(const Vec2f& v, const Vec2f& normal)
	{
		return v - (normal * (dot(v, normal) * 2.0f));
	}

	static Vec2f smallestComponents(const Vec2f& vl, const Vec2f& vr)
	{
		return { std::min(vl.x, vr.x), std::min(vl.y, vr.y) };
	}

	static Vec2f biggestComponents(const Vec2f& vl, const Vec2f& vr)
	{
		return { std::max(vl.x, vr.x), std::max(vl.y, vr.y) };
	}
	 
	static bool equals(const Vec2f& vl, const Vec2f& vr)
	{
		return std::fabs(vl.x - vr.x) <= FLT_EPSILON && std::fabs(vl.y - vr.y) <= FLT_EPSILON;
	}

	static void print(const Vec2f& v) 
	{
		std::cout << std::fixed << std::setprecision(4);
		std::cout << "(" << std::setw(9) << v.x << ", " << std::setw(9) << v.y << ")\n\n";
	}
};

#endif // !SFMATH_H

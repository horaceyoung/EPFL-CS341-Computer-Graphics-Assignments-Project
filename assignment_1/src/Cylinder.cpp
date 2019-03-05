//=============================================================================
//
//   Exercise code for the lecture
//   "Introduction to Computer Graphics"
//   by Prof. Dr. Mario Botsch, Bielefeld University
//
//   Copyright (C) Computer Graphics Group, Bielefeld University.
//
//=============================================================================

//== INCLUDES =================================================================

#include "Cylinder.h"
#include "SolveQuadratic.h"

#include <array>
#include <cmath>

//== IMPLEMENTATION =========================================================

bool
Cylinder::
intersect(const Ray&  _ray,
          vec3&       _intersection_point,
          vec3&       _intersection_normal,
          double&     _intersection_t) const
{
    /** \todo
     * - compute the first valid intersection `_ray` with the cylinder
     *   (valid means in front of the viewer: t > 0)
     * - store intersection point in `_intersection_point`
     * - store ray parameter in `_intersection_t`
     * - store normal at _intersection_point in `_intersection_normal`.
     * - return whether there is an intersection with t > 0
    */
	
	/*  Infinite cylinder along y of radius r axis has equation x^2 + z^2 -r^2 = 0
		The equation for a more general cylinder of radius r oriented along a line c + n*t 
		where c is the center the one surface sphere and n is the surface normal has following:
		(x-c-<n,x-c>*n)^2 - r^2 = 0 where x is a point on the cylinder
		To find the intersection points with a ray o + t*d substitute x = o + t*d and solve:
		(o+t*d-c-<n,o+t*d-c>*n)^2 - r^2 = 0
		reduces to A*t^2 B*t + C = 0 with following:
	*/
	const vec3 oc = _ray.origin - center;

	// A = (d - <d,n> * n)^2
	double A = dot(_ray.direction - dot(_ray.direction, axis)*axis, _ray.direction - dot(_ray.direction, axis)*axis);
	// B = 2 * (d - <d,n> * n, oc - <oc,n> * n)
	double B = 2 * dot(_ray.direction - dot(_ray.direction, axis) * axis, oc - dot(oc, axis) * axis);
	// C = (oc - <oc, n> * n)^2 - r^2
	double C = dot(oc - dot(oc, axis)*axis, oc - dot(oc, axis)*axis) - radius * radius;
	
	std::array<double, 2> t;
	size_t nsol = solveQuadratic(A, B, C, t);
	_intersection_t = NO_INTERSECTION;

	// Find the closest valid solution (in front of the viewer)
	for (size_t i = 0; i < nsol; ++i) {
		if (t[i] > 0) _intersection_t = std::min(_intersection_t, t[i]);
	}

	if (_intersection_t == NO_INTERSECTION) return false;
	_intersection_point = _ray(_intersection_t);
	_intersection_normal = (_intersection_point - center) / radius;

	return true;
}

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
	// B = 2 * <d, oc> - 2 * <n, d> * <n, d>
	double B = 2 * dot(_ray.direction, oc) - 2 * dot(axis, _ray.direction) * dot(axis, _ray.direction);
	// C = (oc - <oc, n> * n)^2 - r^2
	double C = dot(oc - dot(oc, axis)*axis, oc - dot(oc, axis)*axis) - radius * radius;
	
	std::array<double, 2> t = {0, 0};
	size_t nsol = solveQuadratic(A, B, C, t);
	_intersection_t = NO_INTERSECTION;

	/* Select the valid intersections, valid intersections should not have a distance between the intersection point 
	and the center of the cylinder larger than half of the height of the cylinder.
	If any intersection point does not satisfy the above condition, simply mark it as 0
	*/
	for (size_t i = 0; i < nsol; ++i) {
		if (!(t[i] >= 0 && dot((_ray(sol[i]) - center), axis) < height / 2
			&& dot((_ray(sol[i]) - center), axis) > -height / 2)) {
			t[i] = 0; // mark the invalid entries
		}
	}

	/* From the valid intersections, select the one that is nearest to the viewer, i.e., with minimum t
	*/
	for (size_t i = 0; i < nsol; i++) {
		if (t[i] > 0) {
			_intersection_t = std::min(_intersection_t, t[i]);
		}
	}

	/*If no intersections satisfy the conditions, return false*/
	if (_intersection_t = NO_INTERSECTION) return false;

	/*Get the corresponding value from the intersection selected above*/
	_intersection_point = _ray(_intersection_t);
	_intersection_normal = normalize(_ray(t[i]) - center - (dot(_ray.direction, axis)*t[i] + dot(oc, axis)) * axis);

	return true;
}

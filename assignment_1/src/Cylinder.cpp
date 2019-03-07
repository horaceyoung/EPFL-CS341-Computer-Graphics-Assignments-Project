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
#include "Plane.h"

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



	//(d.d - (d.a)^2)t^2 + 2[d.(o-c)-(d.a)((o-c).a)]t + (o-c)^2 - ((o-c).a)^2 - radius^2 = 0
	const vec3 oc = _ray.origin - center;

	//(d.d - (d.a)^2)
	double A = dot(_ray.direction, _ray.direction) - dot(axis, _ray.direction) * dot(axis, _ray.direction);
	//2[d.(o-c)-(d.a)((o-c).a)]
	double B = 2 * (dot(_ray.direction, oc) - dot(_ray.direction, axis) * dot(oc, axis));
	//(o-c)^2 - ((o-c).a)^2 - radius^2
	double C = dot(oc, oc) - dot(oc, axis) * dot(oc, axis) - radius * radius;

	std::array<double, 2> sol = { 0,0 };
	size_t sol_num = solveQuadratic(A, B, C, sol);
	int intersect_num = 0;

	std::array<double, 2> t = { 0, 0 };
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
	int choose = 0;
	for (size_t i = 0; i < nsol; i++) {
		if (t[i] > 0) {
			_intersection_t = std::min(_intersection_t, t[i]);
			choose = int(i);
		}
	}

	/*If no intersections satisfy the conditions, return false*/
	if (_intersection_t = NO_INTERSECTION) return false;

	/*Get the corresponding value from the intersection selected above*/
	_intersection_point = _ray(_intersection_t);
	_intersection_normal = normalize(_ray(t[choose]) - center - (dot(_ray.direction, axis)*t[choose] + dot(oc, axis)) * axis);

	return true;
}
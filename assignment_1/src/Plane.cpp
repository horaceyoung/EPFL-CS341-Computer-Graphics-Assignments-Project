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

#include "Plane.h"
#include <limits>


//== CLASS DEFINITION =========================================================



Plane::Plane(const vec3& _center, const vec3& _normal)
: center(_center), normal(_normal)
{
}


//-----------------------------------------------------------------------------


bool
Plane::
intersect(const Ray& _ray,
          vec3&      _intersection_point,
          vec3&      _intersection_normal,
          double&    _intersection_t ) const
{
    /** \todo
     * - compute the intersection of the plane with `_ray`
     * - if ray and plane are parallel there is no intersection
     * - otherwise compute intersection data and store it in `_intersection_point`, `_intersection_normal`, and `_intersection_t`.
     * - return whether there is an intersection in front of the viewer (t > 0)
    */
	
	// 1.Compute the dot product of surface normal and the ray's direction. If the result == 0, aka ray and surface normal are paralel, return false;
	double isPara = dot(_ray.direction, normal);
	if (isPara == 0) {
		return false;
	}

	_intersection_t = NO_INTERSECTION;

	// 2.Else, solve for t 
	_intersection_t = (dot(normal, center) - dot(normal, _ray.origin)) / (dot(normal, _ray.direction));
	if (_intersection_t <= 0) return false; //return false if the intersection point is behind the viewer.
	else {
		_intersection_point = _ray(_intersection_t);
		_intersection_normal = normal;
		return true;
	}
}


//=============================================================================

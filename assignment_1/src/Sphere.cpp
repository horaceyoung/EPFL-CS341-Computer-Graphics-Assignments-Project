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

#include "Sphere.h"
#include "SolveQuadratic.h"

//== IMPLEMENTATION =========================================================


Sphere::Sphere(const vec3& _center, double _radius)
: center(_center), radius(_radius)
{
}


//-----------------------------------------------------------------------------


bool
Sphere::
intersect(const Ray&  _ray,
          vec3&       _intersection_point,
          vec3&       _intersection_normal,
          double&     _intersection_t) const
{

    const vec3 &dir = _ray.direction;
    const vec3   oc = _ray.origin - center;

    std::array<double, 2> t;
    size_t nsol = solveQuadratic(dot(dir, dir),
                                 2 * dot(dir, oc),
                                 dot(oc, oc) - radius * radius, t);

    _intersection_t = NO_INTERSECTION;

    // Find the closest valid solution (in front of the viewer)
    

    return true;
}

//=============================================================================

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

#include "Mesh.h"
#include <fstream>
#include <string>
#include <stdexcept>
#include <limits>


//== IMPLEMENTATION ===========================================================


Mesh::Mesh(std::istream &is, const std::string &scenePath)
{
    std::string meshFile, mode;
    is >> meshFile;

    // load mesh from file
    read(scenePath.substr(0, scenePath.find_last_of("/\\") + 1) + meshFile); // Use both Unix and Windows path separators

    is >> mode;
    if      (mode ==  "FLAT") draw_mode_ = FLAT;
    else if (mode == "PHONG") draw_mode_ = PHONG;
    else throw std::runtime_error("Invalid draw mode " + mode);

    is >> material;
}


//-----------------------------------------------------------------------------


bool Mesh::read(const std::string &_filename)
{
    // read a mesh in OFF format


    // open file
    std::ifstream ifs(_filename);
    if (!ifs)
    {
        std::cerr << "Can't open " << _filename << "\n";
        return false;
    }


    // read OFF header
    std::string s;
    unsigned int nV, nF, dummy, i;
    ifs >> s;
    if (s != "OFF")
    {
        std::cerr << "No OFF file\n";
        return false;
    }
    ifs >> nV >> nF >> dummy;
    std::cout << "\n  read " << _filename << ": " << nV << " vertices, " << nF << " triangles";


    // read vertices
    Vertex v;
    vertices_.clear();
    vertices_.reserve(nV);
    for (i=0; i<nV; ++i)
    {
        ifs >> v.position;
        vertices_.push_back(v);
    }


    // read triangles
    Triangle t;
    triangles_.clear();
    triangles_.reserve(nF);
    for (i=0; i<nF; ++i)
    {
        ifs >> dummy >> t.i0 >> t.i1 >> t.i2;
        triangles_.push_back(t);
    }


    // close file
    ifs.close();


    // compute face and vertex normals
    compute_normals();

    // compute bounding box
    compute_bounding_box();


    return true;
}


//-----------------------------------------------------------------------------

// Determine the weights by which to scale triangle (p0, p1, p2)'s normal when
// accumulating the vertex normals for vertices 0, 1, and 2.
// (Recall, vertex normals are a weighted average of their incident triangles'
// normals, and in our raytracer we'll use the incident angles as weights.)
// \param[in] p0, p1, p2    triangle vertex positions
// \param[out] w0, w1, w2    weights to be used for vertices 0, 1, and 2
void angleWeights(const vec3 &p0, const vec3 &p1, const vec3 &p2,
                  double &w0, double &w1, double &w2) {
    // compute angle weights
    const vec3 e01 = normalize(p1-p0);
    const vec3 e12 = normalize(p2-p1);
    const vec3 e20 = normalize(p0-p2);
    w0 = acos( std::max(-1.0, std::min(1.0, dot(e01, -e20) )));
    w1 = acos( std::max(-1.0, std::min(1.0, dot(e12, -e01) )));
    w2 = acos( std::max(-1.0, std::min(1.0, dot(e20, -e12) )));
}


//-----------------------------------------------------------------------------

void Mesh::compute_normals()
{
    // compute triangle normals
    for (Triangle& t: triangles_)
    {
        const vec3& p0 = vertices_[t.i0].position;
        const vec3& p1 = vertices_[t.i1].position;
        const vec3& p2 = vertices_[t.i2].position;
        t.normal = normalize(cross(p1-p0, p2-p0));
    }

    // initialize vertex normals to zero
    for (Vertex& v: vertices_)
    {
        v.normal = vec3(0,0,0);
    }

    /** \todo
     * In some scenes (e.g the office scene) some objects should be flat
     * shaded (e.g. the desk) while other objects should be Phong shaded to appear
     * realistic (e.g. chairs). You have to implement the following:
     * - Compute vertex normals by averaging the normals of their incident triangles.
     * - Store the vertex normals in the Vertex::normal member variable.
     * - Weigh the normals by their triangles' angles.
     */


    /*for (Triangle& t: triangles_)
    {
        const vec3& p0 = vertices_[t.i0].position;
        const vec3& p1 = vertices_[t.i1].position;
        const vec3& p2 = vertices_[t.i2].position;
        
        double w0, w1, w2;
        angleWeights(p0,p1,p2,w0,w1,w2);

        vertices_[t.i0].normal += w0 * t.normal;
        vertices_[t.i1].normal += w1 * t.normal;
        vertices_[t.i2].normal += w2 * t.normal;
    }

    for(Vertex& v:vertices_)
        v.normal=normalize(v.normal); */

	//Traverse through all the triangles, compute corresponding weights and add the weighted normal to each vertex respectively
	for (Triangle& t: triangles_) {
		double w0, w1, w2;
		const vec3 p0 = vertices_[t.i0].position;
		const vec3 p1 = vertices_[t.i1].position;
		const vec3 p2 = vertices_[t.i2].position;
		angleWeights(p0, p1, p2, w0, w1, w2);
		vertices_[t.i0].normal += w0 * t.normal;
		vertices_[t.i1].normal += w1 * t.normal;
		vertices_[t.i2].normal += w2 * t.normal;
	}

	//Traverse all the vertices, normalize the normal
	for (Vertex& v : vertices_) {
		v.normal = normalize(v.normal);
	}

}


//-----------------------------------------------------------------------------


void Mesh::compute_bounding_box()
{
    bb_min_ = vec3(std::numeric_limits<double>::max());
    bb_max_ = vec3(std::numeric_limits<double>::lowest());

    for (Vertex v: vertices_)
    {
        bb_min_ = min(bb_min_, v.position);
        bb_max_ = max(bb_max_, v.position);
    }
}


//-----------------------------------------------------------------------------

bool intersect_box_face(const vec3& min_p, const vec3& max_p, const Ray& _ray, int axis) {
	double t = (min_p[axis] - _ray.origin[axis]) / _ray.direction[axis];
	if (t < 0)
		return false;
	vec3 p = _ray(t);
	double err = 1e-4;
	for (int i = 0; i < 3; i++)
		if (i != axis)
			if (p[i] < min_p[i] - err || p[i] > max_p[i] + err)
				return false;
	return true;

}

bool Mesh::intersect_bounding_box(const Ray& _ray) const
{

    /** \todo
    * Intersect the ray `_ray` with the axis-aligned bounding box of the mesh.
    * Note that the minimum and maximum point of the bounding box are stored
    * in the member variables `bb_min_` and `bb_max_`. Return whether the ray
    * intersects the bounding box.
    * This function is ued in `Mesh::intersect()` to avoid the intersection test
    * with all triangles of every mesh in the scene. The bounding boxes are computed
    * in `Mesh::compute_bounding_box()`.
    */
	vec3 move_max;
	vec3 move_min;
	for (int axis = 0; axis < 3; axis++) {
		move_max = bb_max_;
		move_max[axis] = bb_min_[axis];
		move_min = bb_min_;
		move_min[axis] = bb_max_[axis];
		if (intersect_box_face(move_min, bb_max_, _ray, axis) || intersect_box_face(bb_min_, move_max, _ray, axis))
			return true;
	}
	return false;
    return true;
}


//-----------------------------------------------------------------------------


bool Mesh::intersect(const Ray& _ray,
                     vec3&      _intersection_point,
                     vec3&      _intersection_normal,
                     double&    _intersection_t ) const
{
    // check bounding box intersection
    if (!intersect_bounding_box(_ray))
    {
        return false;
    }

    vec3   p, n;
    double t;

    _intersection_t = NO_INTERSECTION;

    // for each triangle
    for (const Triangle& triangle : triangles_)
    {
        // does ray intersect triangle?
        if (intersect_triangle(triangle, _ray, p, n, t))
        {
            // is intersection closer than previous intersections?
            if (t < _intersection_t)
            {
                // store data of this intersection
                _intersection_t      = t;
                _intersection_point  = p;
                _intersection_normal = n;
            }
        }
    }

    return (_intersection_t != NO_INTERSECTION);
}

//-----------------------------------------------------------------------------

double Mesh::determinant(double a, double b, double c, double d, double e, double f, double g, double h, double i) const
{
	return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
}

//-----------------------------------------------------------------------------


bool
Mesh::
intersect_triangle(const Triangle&  _triangle,
                   const Ray&       _ray,
                   vec3&            _intersection_point,
                   vec3&            _intersection_normal,
                   double&          _intersection_t) const
{
    const vec3& p0 = vertices_[_triangle.i0].position;
    const vec3& p1 = vertices_[_triangle.i1].position;
    const vec3& p2 = vertices_[_triangle.i2].position;

    /** \todo
    * - intersect _ray with _triangle
    * - store intersection point in `_intersection_point`
    * - store ray parameter in `_intersection_t`
    * - store normal at intersection point in `_intersection_normal`.
    * - Depending on the member variable `draw_mode_`, use either the triangle
    *  normal (`Triangle::normal`) or interpolate the vertex normals (`Vertex::normal`).
    * - return `true` if there is an intersection with t > 0 (in front of the viewer)
    *
    * Hint: Rearrange `ray.origin + t*ray.dir = a*p0 + b*p1 + (1-a-b)*p2` to obtain a solvable
    * system for a, b and t.
    * Refer to [Cramer's Rule](https://en.wikipedia.org/wiki/Cramer%27s_rule) to easily solve it.
     */


    /*vec3 v1 = p1 - p0;
    vec3 v2 = p2 - p0;
    vec3 v3 = - _ray.direction;
    vec3 v4 = _ray.origin - p0;

    double det = dot(v1, cross(v2, v3));
    double det_1 =  dot(v4, cross(v2, v3));
    double det_2 = dot(v1, cross(v4, v3));
    double det_3 = dot(v1, cross(v2, v4));

    if(det < 0)         // -1e-4< && <1e-4
        return false;

    double a = det_1/det;
    double b = det_2/det;
    double r = 1-a-b;
    double t = det_3/det;

     if(t < 0 || a < 0 || b < 0 || r < 0 )
        return false;

    _intersection_t = t;
    _intersection_point = _ray(_intersection_t);

    if(draw_mode_ == FLAT)
        _intersection_normal = normalize(_triangle.normal);
    else
        _intersection_normal = normalize(a * vertices_[_triangle.i0].normal + b * vertices_[_triangle.i1].normal 
                                            + r * vertices_[_triangle.i2].normal);
    
    if(dot(_intersection_normal, _ray.direction) < 0) // 1e-2 ?
        _intersection_normal = - _intersection_normal;

    return true;*/


	/*  Drivation
		ray.origin + t*ray.dir = a*p0 + b*p1 + (1-a-b)*p2 ->
	 	ray.origin + t*ray.dir = a*p0 + b*p1 +p2 - a*p2 - b*p2 ->
		ray.origin + t*ray.dir = a*(p0-p2) + b*(p1-p2) + p2 ->
		a*(p2-p0) + b*(p2-p1) + ray.dir*t = p2 - ray.origin ->
								|a|
		[p2-p0 p2-p1 ray.dir] * |b| = [p2 - ray.origin] ->
								|t|

		|xp2-xp0 xp2-xp1 xd|   |a|   |xp2 - xo|
		|yp2-yp0 yp2-yp1 yd| * |b| = |yp2 - yo|
		|zp2-zp0 zp2-zp1 zd|   |t| 	 |zp2 - zo| A solvable linear system
	*/

	//Use Cramer's Rule to solve the above linear system
	vec3 v0 = vertices_[_triangle.i0].position;
	vec3 v1 = vertices_[_triangle.i1].position;
	vec3 v2 = vertices_[_triangle.i2].position;
	vec3 col1 = v2 - v0;
	vec3 col2 = v2 - v1;
	vec3 col3 = _ray.direction;
	vec3 res = v2 - _ray.origin;


	double determinant_original = dot(cross(col1,col2), col3);
	if (determinant_original < 1e-4 && determinant_original > -1e-4)
		return false;
	double alpha = dot(cross(res, col2), col3) / determinant_original;
	double beta = dot(cross(col1, res), col3) / determinant_original;
	double t = dot(cross(col1, col2), res) / determinant_original;

	if (alpha<0 || beta <0 || (1-alpha-beta)<0 || t<0)
		return false;
	else {
		_intersection_t = t;
		_intersection_point = _ray(_intersection_t);
		if (draw_mode_ == FLAT) {
			_intersection_normal = normalize(_triangle.normal);
		}
		else if (draw_mode_ == PHONG) {
			_intersection_normal = normalize(alpha * vertices_[_triangle.i0].normal + beta * vertices_[_triangle.i1].normal + (1 - alpha - beta) * vertices_[_triangle.i2].normal);
		}

		return true;
	}

}




//=============================================================================

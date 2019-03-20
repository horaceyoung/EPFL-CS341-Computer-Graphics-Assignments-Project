1. Vertex Normals
Traverse through all the triangles, compute corresponding weights and add the weighted normal to each vertex respectively. And then traverse all the vertices, normalize the normal.

2.  Ray-triangle intersection + normal calculation
Rearrange `ray.origin + t*ray.dir = a*p0 + b*p1 + (1-a-b)*p2` to obtain a solvable
 system for a, b and t.
Drivation
		
ray.origin + t*ray.dir = a*p0 + b*p1 + (1-a-b)*p2 ->
	 	
ray.origin + t*ray.dir = a*p0 + b*p1 +p2 - a*p2 - b*p2 ->
		
ray.origin + t*ray.dir = a*(p0-p2) + b*(p1-p2) + p2 ->
		
a*(p2-p0) + b*(p2-p1) + ray.dir*t = p2 - ray.origin ->
								
                                  |a|
		
[p2-p0 p2-p1 ray.dir] * |b| = [p2 - ray.origin] ->
								
                                  |t|

	
	
|xp2-xp0 xp2-xp1 xd|    |a|    |xp2 - xo|
		
|yp2-yp0 yp2-yp1 yd| * |b| = |yp2 - yo|
		
|zp2-zp0 zp2-zp1   zd|   |t |    |zp2 - zo| A solvable linear system, then apply cramer's rule and use the cross product to solve for alpha, beta, and t.
determinant_original = dot(cross(col1,col2), col3)	
alpha = dot(cross(res, col2), col3) / determinant_original	
beta = dot(cross(col1, res), col3) / determinant_original
	
t = dot(cross(col1, col2), res) / determinant_original;

if the draw mode is FLAT, the intersection normal is normalized triangle normal.
if the draw mode is PHONG, the intersection normal is normalized weighted normal.

3. Bounding box intersection
For each axis (x, y, z), define a moved vec3 move_max and a move_min to move bb_max_, and bb_min_ to the same plane respectively.
Then for that particular axis, determine the t value that draws the ray to the same plane as the faces of the bounding box by :
t = (min[axis] - _ray.origin[axis]) / _ray.direction[axis] where min is a point on the plane.
Compute the intersection point on the plane, then for each axis, determine if the intersection point falls inside the range of the faces:
vec3 r = _ray(t);
	
double error = 1e-4;
	
for (int i = 0; i < 3; i++) {
		
if (i != axis)
			
	if (r[i] < min[i] - error || r[i] > max[i] + error)
				
		return false;
	
}

4. Work Distribution
Yong Hao 50%
Zhang Yuehan 50%

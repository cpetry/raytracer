#pragma once
class Vector
{
public:
	double x,y,z;
	double normal;
public:
	Vector(void) : x(0.0), y(0.0), z(0.0) {};
	Vector(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {};

	static Vector calculateNormal(Vector a, Vector b, Vector c){
		Vector u = b.vsub(a);
		Vector v = c.vsub(a);
 
		Vector normal = Vector((u.y * v.z - u.z * v.y),
						(u.z * v.x - u.x * v.z),
						(u.x * v.y - u.y * v.x));
		return normal;
	}
	
	Vector cross (Vector &v);
	double dot(Vector &v);
	Vector normalize(void);
	Vector svmpy(double s);
	Vector vadd(Vector &v); 
	double veclength (void);
	Vector vsub(Vector &v);
};


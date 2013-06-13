#pragma once
#include "Surface.h"
#include "Property.h"
#include "Vector.h"
#include "vector"


class Objekt
{
	Surface* surface;
	Property *properties;
	std::vector<Vector> *normals;
	Vector normal;

public:
	Objekt(void) : properties(NULL) {};
	Objekt(Surface* s, Property *p) : surface(s), properties(p) {};
	Objekt(Surface* s, std::vector<Vector> *normals, Property *p) : surface(s), normals(normals), properties(p) {};

	Property* getProperty()  { return properties; };
	Surface* getSurface()  { return surface; };

	Vector get_normal(Vector &v, bool gouraud_shaded);
	void set_normal(Vector &v){ normal = v;};
};


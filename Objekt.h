#pragma once
#include "Surface.h"
#include "Property.h"
#include "Vector.h"
#include "vector"


class Objekt
{
	Surface *surface;
	Property *properties;
	Vector normal;
	Vector averaged_normal[3];

public:
	Objekt(void) : surface(NULL), properties(NULL) {};
	Objekt(Surface *s, Property *p) : surface(s), properties(p) {};

	Property* getProperty()  { return properties; };
	Surface* getSurface()  { return surface; };

	Vector get_normal(Vector &v);
	void set_normal(Vector v){ normal = v;};
};


#pragma once

#include "string"
#include <vector>
#include "Vector.h"

static enum surface_type {QUADRIC, POLYGON, POLY};

class Surface
{
public:
	
	Vector v1,v2,v3;
	double a, b, c, d, e, f, g, h, j, k;
	std::vector<Surface> polygons;
	std::string name;
	surface_type type;

	Surface(void) : a(0.0), b(0.0), c(0.0), d(0.0), e(0.0), f(0.0), g(0.0), h(0.0), j(0.0), k(0.0) { name = ""; };
	Surface(char *n, double _a, double _b, double _c, double _d, double _e, double _f, double _g, double _h, double _j, double _k) : 
	a(_a), b(_b), c(_c), d(_d), e(_e), f(_f), g(_g), h(_h), j(_j), k(_k) { name = n; type = QUADRIC;};

	Surface(Vector v1, Vector v2, Vector v3) : v1(v1), v2(v2), v3(v3) {name = ""; type = POLYGON;};
	Surface(char *n, std::vector<Surface> polygons) : polygons(polygons){ name = n; type = POLY;};

	virtual std::string getName() { return name; };
	surface_type getType() { return type; };

};


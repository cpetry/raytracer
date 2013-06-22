#pragma once

#include "string"
#include <vector>

#include "Vector.h"

static enum surface_type {QUADRIC, POLYGON, POLY};

class Surface
{
public:
	
	std::vector<Vector> vertex_index;
	std::vector<Vector*> average_normals;
	double a, b, c, d, e, f, g, h, j, k;
	std::vector<Surface> polygons;
	std::string name;
	int number;
	surface_type type;

	Surface(void) : a(0.0), b(0.0), c(0.0), d(0.0), e(0.0), f(0.0), g(0.0), h(0.0), j(0.0), k(0.0), name(""){};
	Surface(char *n, double _a, double _b, double _c, double _d, double _e, double _f, double _g, double _h, double _j, double _k) : 
		a(_a), b(_b), c(_c), d(_d), e(_e), f(_f), g(_g), h(_h), j(_j), k(_k), name(n), type(QUADRIC){ number = atoi(std::string(n).substr(1).c_str());};

	Surface(char* n, Vector* vertex0, Vector* vertex1, Vector* vertex2, Vector* normal0, Vector* normal1, Vector* normal2) : name(n), type(POLYGON) 
	{
		number = atoi(std::string(n).substr(1).c_str());
		vertex_index.push_back(*vertex0); vertex_index.push_back(*vertex1); vertex_index.push_back(*vertex2);
		average_normals.push_back(normal0); average_normals.push_back(normal1); average_normals.push_back(normal2);
	};

	Surface(char *n, std::vector<Surface> polygons) : polygons(polygons), name(n), type(POLY){ number = atoi(std::string(n).substr(1).c_str()); };

	virtual std::string getName() { return name; };
	int getNumber(){ return number; };
	surface_type getType() { return type; };

};


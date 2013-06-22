#pragma once

#include <vector>

#include "Vector.h"
#include "Color.h"

class File;
class Surface;
class Light;
class Objekt;

class Ray
{
	Vector direction, origin;
	int	 depth, maxdepth;

	Color shaded_color(Light *light, Ray &reflectedray, Vector &normal, Objekt *obj);

public:
	Ray(void) : direction(), origin(), depth (0) {};
	Ray(Vector &dir, Vector &orig, int d) : direction(dir), origin(orig), depth (d) {};
	Ray(Vector &dir, Vector &orig, int d, int maxdepth) : direction(dir), origin(orig), depth(d), maxdepth(maxdepth) {};
	Color shade(File* file);

	Vector getDirection() { return direction; };
	Vector getOrigin() { return origin; };
	int getDepth() { return depth; };

	void setDirection(Vector &v) { direction=v; };
	void setOrigin(Vector &v) { origin=v; };
	void setDepth(int d) { depth=d; };

	Ray reflect(Vector &origin, Vector &normal);
	double intersect(Objekt &obj);
	double intersectQuadric(Objekt &obj);
	double intersectPolygon(Objekt &obj);
};



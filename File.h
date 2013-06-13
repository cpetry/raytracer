#pragma once

#include <string>
#include <vector>
#include <map>

#include <QString>

#include "Color.h"
#include "Surface.h"
#include "Property.h"
#include "Objekt.h"
#include "Light.h"

class File{
public:
	std::vector<Surface> surfaces;
	std::vector<Property> properties;
	std::vector<Vector> vertices;
	std::vector<Vector> averaged_normals;
	std::vector<std::vector<int>> indices;
	std::vector<Objekt> objekte;
	std::vector<Light> lights;
	int resolutionX, resolutionY, bounces, super_sampling;
	double fovy, aspect;
	Vector eye, lookat, up;
	Color background, ambience;
	QString filename;
	bool gouraud_shaded;

	File() : bounces(0), super_sampling(0), gouraud_shaded(true) {};

	File* openFile(QString filename);
};
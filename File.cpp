#include "File.h"

#include <vector>

#include "Color.h"
#include "Vector.h"
#include "Surface.h"
#include "Property.h"
#include "Objekt.h"
#include "Light.h"

Color background, ambience;

extern "C" {
	extern FILE *yyin;
	int yyparse();

	File* file = new File();

	void add_light(char *n, double dirx, double diry, double dirz, double colr, double colg, double colb) {
		fprintf(stderr,"  adding light %f %f %f %f %f %f\n", dirx, diry, dirz, colr, colg, colb);
		file->lights.push_back(Light(Vector(dirx,diry,dirz).normalize(), Color(colr, colg, colb)));
	};
	void add_quadric(char *n, double a, double b, double c, double d, double e, double f, double g, double h, double j, double k) {
		fprintf(stderr,"  adding quadric %s %f %f %f %f %f %f %f %f %f %f %f %f\n", n, a,b,c,d,e,f,g,h,j,k);
		file->surfaces.push_back(Surface(n, a,b,c,d,e,f,g,h,j,k));
	};
	void add_property(char *n,  double ar, double ag, double ab, double r, double g, double b, double s, double ss, double m) {
		fprintf(stderr,"  adding prop %f %f %f %f %f\n", r, g, b, s, m);
		file->properties.push_back(Property(n, Color(ar, ag, ab), Color(r, g, b), s, ss, m));
	};
	void add_vertex(double x, double y, double z){
		fprintf(stderr,"  adding vector %f %f %f\n", x, y, z);
		file->vertices.push_back(Vector(x,y,z));
	}
	void add_index_array(){
		file->indices.push_back(std::vector<int>());
	}
	void add_index(int i){
		file->indices.back().push_back(i);
	}
	void add_objekt(char *ns, char *np) {
		Surface *s = NULL;
		Property *p = NULL;
		std::string ss(ns);
		std::string sp(np);

		for(std::vector<Surface>::iterator i = file->surfaces.begin(); i != file->surfaces.end(); ++i) 
			if(i->getName() == ss) {
				s = &(*i);
				break;
			}
		for(std::vector<Property>::iterator i = file->properties.begin(); i != file->properties.end(); ++i) 
			if(i->getName() == sp) {
				p = &(*i);
				break;
			}

		if(s == NULL) {
			fprintf(stderr, "Surface not found: %s\n", ns);
			exit(1);
		}
		if(p == NULL) {
			fprintf(stderr, "Property not found: %s\n", np);
			exit(1);
		}

		if (s->getType() == surface_type::POLY)
			for (Surface polygon : s->polygons){
				file->objekte.push_back(Objekt(new Surface(polygon), p));
			}
		else
			file->objekte.push_back(Objekt(s, p));
		file->vertices.clear();
		fprintf(stderr, "  adding object: surface %s, property %s\n", ns, np);
	}
	void add_polygon(char *n){
		std::vector<Surface> polygons;
		while(file->indices.size() > 0){
			Vector vertices[3];
			for (int i=0; i < 3; i++)
				vertices[i] = file->vertices.at(file->indices.back().at(i)-1);
			polygons.push_back(Surface(vertices[0], vertices[1], vertices[2]));

			if (file->indices.back().size() == 4){
				vertices[0] = file->vertices.at(file->indices.back().at(2)-1);
				vertices[1] = file->vertices.at(file->indices.back().at(3)-1);
				vertices[2] = file->vertices.at(file->indices.back().at(0)-1);
				polygons.push_back(Surface(vertices[0], vertices[1], vertices[2]));
			}
			file->indices.pop_back();
		}
		file->surfaces.push_back(Surface(n, polygons));
	}
	void add_resolution(int resx, int resy){
		file->resolutionX = resx;
		file->resolutionY = resy;
	}
	void add_background(double colr, double colg, double colb){
		file->background.r = colr;
		file->background.g = colg;
		file->background.b = colb;
	}
	void add_eye_position(double eyex, double eyey, double eyez){
		file->eye.x = eyex;
		file->eye.y = eyey;
		file->eye.z = eyez;
	}
	void add_lookat(double lookatx, double lookaty, double lookatz){
		file->lookat.x = lookatx;
		file->lookat.y = lookaty;
		file->lookat.z = lookatz;
	}
	void add_fovy(double infovy){
		file->fovy = infovy;
	}
	void add_up(double upx, double upy, double upz){
		file->up.x = upx;
		file->up.y = upy;
		file->up.z = upz;
	}
	void add_aspect(double inaspect){
		file->aspect = inaspect;
	}
	void add_ambience(double ambr, double ambg, double ambb){
		file->ambience.r = ambr;
		file->ambience.g = ambg;
		file->ambience.b = ambb;
	}
}

File* File::openFile(QString filename){
	yyin = fopen(filename.toUtf8().data(),"r");
	if(yyin == NULL) {
		fprintf(stderr, "Error: Konnte Datei nicht öffnen\n");
		system("PAUSE");
	}
	yyparse();
	fclose (yyin);
	
	*this = *file;
	
	background = this->background;
	ambience = this->ambience;

	return file;
}
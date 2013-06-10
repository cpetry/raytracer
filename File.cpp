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
	void add_property(char *n,  double ar, double ag, double ab, double r, double g, double b, double s, double m) {
		fprintf(stderr,"  adding prop %f %f %f %f %f\n", r, g, b, s, m);
		file->properties.push_back(Property(n, Color(ar, ag, ab), Color(r, g, b), s, m));
	};
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
		file->objekte.push_back(Objekt(s, p));
		fprintf(stderr, "  adding object: surface %s, property %s\n", ns, np);
	}
	void add_sphere(double x, double y, double z, double radius) {
		Surface *s = NULL;
		Property *p = NULL;

		//D = -2Xm , G = -2Ym , J = -2Zm und K = Xm^2 + Ym^2 + Zm^2 - Radius^2
		s = new Surface("sphere", -2*x,0,0,0, -2*y,0,0, -2*z,0, x*x + y*y + z*z - radius*radius);
		p = new Property("sphereprop", Color(0.5, 0, 0), Color(1, 0, 0), 0.2, 0.1);

		if(s == NULL) {
			fprintf(stderr, "Surface not found: %s\n", s);
			exit(1);
		}
		file->objekte.push_back(Objekt(s, p));
		fprintf(stderr, "  adding object: surface %s, property %s\n", s, p);
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
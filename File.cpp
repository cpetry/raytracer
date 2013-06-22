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

	File* file;
	std::string current_Object_name = "";

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
		//fprintf(stderr,"  adding vector %f %f %f\n", x, y, z);
		file->vertices.push_back(Vector(x,y,z));
		file->averaged_normals.push_back(Vector(0,0,0));
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

		if (s->getType() == surface_type::POLY){
			double maxX=s->polygons.at(0).vertex_index.at(0).x;
			double maxY=s->polygons.at(0).vertex_index.at(0).y; 
			double maxZ=s->polygons.at(0).vertex_index.at(0).z; 
			double minX=maxX;
			double minY=maxY;
			double minZ=maxZ;

			for (Surface polygon : s->polygons){
				Surface* s = new Surface(polygon);
				for (int i=0;i<3;i++){
					maxX = std::max(polygon.vertex_index.at(i).x, maxX);
					maxY = std::max(polygon.vertex_index.at(i).y, maxY);
					maxZ = std::max(polygon.vertex_index.at(i).z, maxZ);

					minX = std::min(polygon.vertex_index.at(i).x, minX);
					minY = std::min(polygon.vertex_index.at(i).y, minY);
					minZ = std::min(polygon.vertex_index.at(i).z, minZ);
				}
				file->objekte.push_back(Objekt(s, &file->averaged_normals, p));
				//fprintf(stderr, "  adding object: surface %s, property %s\n", ns, np);
			}
		
			double posX, posY, posZ, radius;
			posX = (maxX+minX)/2;
			posY = (maxY+minY)/2;
			posZ = (maxZ+minZ)/2;
			radius = Vector(maxX, maxY, maxZ).vsub(Vector(posX,posY,posZ)).veclength();
			file->bounding_volumes.push_back(Objekt(new Surface(ns, 1, 0, 0, -2*posX, 1, 0, -2*posY, 1, -2*posZ, posX*posX + posY*posY + posZ*posZ - radius*radius), new Property()));			
		}

		else{
			file->objekte.push_back(Objekt(s, p));
			file->bounding_volumes.push_back(Objekt(s, new Property()));
		}

		
		
		//file->averaged_normals.clear();
		//fprintf(stderr, "  adding object: surface %s, property %s\n", ns, np);
	}
	void add_polygon(char *n){
		std::vector<Surface> triangles;
		while(file->indices.size() > 0){

			int position[3] = {0,1,2};

			//calculating normal for polygon
			Vector normal = Vector::calculateNormal(file->vertices.at(file->indices.back().at(position[0])-1), 
													file->vertices.at(file->indices.back().at(position[1])-1), 
													file->vertices.at(file->indices.back().at(position[2])-1));

			// adding normal of polygon to averaged_normals
			for (int i=0; i<3; i++)
				file->averaged_normals.at(file->indices.back().at(position[i])-1) = file->averaged_normals.at(file->indices.back().at(position[i])-1).vadd(normal);

			// creating triangle
			triangles.push_back(Surface(n, &file->vertices.at(file->indices.back().at(position[0])-1),
										&file->vertices.at(file->indices.back().at(position[1])-1), 
										&file->vertices.at(file->indices.back().at(position[2])-1),
										&file->averaged_normals.at(file->indices.back().at(position[0])-1),
										&file->averaged_normals.at(file->indices.back().at(position[1])-1),
										&file->averaged_normals.at(file->indices.back().at(position[2])-1)
										));
			
			if (file->indices.back().size() == 4){
				int position[3] = {2,3,0};
				//calculating normal for polygon
				Vector normal = Vector::calculateNormal(file->vertices.at(file->indices.back().at(position[0])-1), 
														file->vertices.at(file->indices.back().at(position[1])-1), 
														file->vertices.at(file->indices.back().at(position[2])-1));

				// adding normal of polygon to averaged_normals
				for (int i=0; i<3; i++)
					file->averaged_normals.at(file->indices.back().at(position[i])-1) = file->averaged_normals.at(file->indices.back().at(position[i])-1).vadd(normal);

				// creating triangle
				triangles.push_back(Surface(n, &file->vertices.at(file->indices.back().at(position[0])-1),
											&file->vertices.at(file->indices.back().at(position[1])-1), 
											&file->vertices.at(file->indices.back().at(position[2])-1),
											&file->averaged_normals.at(file->indices.back().at(position[0])-1),
											&file->averaged_normals.at(file->indices.back().at(position[1])-1),
											&file->averaged_normals.at(file->indices.back().at(position[2])-1)
											));
			}
			file->indices.pop_back();
		}
		
		file->surfaces.push_back(Surface(n, triangles));
		file->vertices.clear();
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
	file = new File();
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
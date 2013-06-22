#include "File.h"
#include "Ray.h"

#include <math.h>
#include <float.h>
#include <vector>

/*----------------------------------------------------------------------------*/
/* shade   arbeitet den folgenden Algorithmus ab:                             */
/*   Schneide den Sichtstrahl mit jedem Objekt. Merke, welches Objekt den     */
/*   kleinsten Parameterwert fuer den Schnittpunkt liefert. Wurde kein Objekt */
/*   geschnitten, dann setze den Farbwert auf Hintergrund. Von dem kleinsten  */
/*   Schnittpunkt aus, bestimme Strahl zu jeder Lichtquelle. Trifft dieser    */
/*   Strahl ein Objekt, dann traegt diese Lichtquelle nicht zur Beleuchtung   */
/*   bei. Bestimme den Farbwert nach der Lichtgleichung.                      */
/*   Dann berechne den Reflektionsstrahl und fuehre Beleuchtungsberechnung    */
/*   auch fuer diesen Strahl durch und addiere dessen Farbwerte zu den        */
/*   bereits bestimmten Farbwerten.                                           */
/* Rueckgabeparameter: Farbe, die auf diesem Strahl zu sehen ist              */
/*----------------------------------------------------------------------------*/

Color Ray::shade(File* file)
{
	Objekt *closest = NULL;
	Color cur_color; 
	double min_t = DBL_MAX, t;

	Vector intersection_position,	normal;
	Ray lv, reflected_ray;
	bool something_intersected = false;

	std::vector<int> volumes_hit;

	for (std::vector<Objekt>::iterator o = file->bounding_volumes.begin(); o != file->bounding_volumes.end(); ++o) {
		t = intersect(*o);
		if (0.0 < t) {
			volumes_hit.push_back(o->getSurface()->getNumber());
		}
	}
	

	for (std::vector<Objekt>::iterator o = file->objekte.begin(); o != file->objekte.end(); ++o) {
		for (std::vector<int>::iterator s = volumes_hit.begin(); s != volumes_hit.end(); ++s) {
			if (o->getSurface()->getNumber() == *s){
				t = intersect(*o);
				if (0.0 < t && t < min_t) {
					min_t = t;
					closest = &(*o);
				}
			}
		}
	}

	volumes_hit.clear();

	if (closest == NULL) {
		if (depth == 0)
			cur_color = file->background; //background_color;
		else
			cur_color = black;
	} else {
		intersection_position = origin.vadd(direction.svmpy(min_t));
		normal = closest->get_normal(intersection_position, file->gouraud_shaded);
		reflected_ray = reflect(intersection_position, normal);
		cur_color = closest->getProperty()->getAmbient().outprodc(file->ambience);  // black statt Globales Ambient -> nun ambient

		for (std::vector<Light>::iterator li = file->lights.begin(); li != file->lights.end(); ++li) {
			lv.setDirection(li->getDirection());
			lv.setOrigin(intersection_position);
			something_intersected = false;

			for (std::vector<Objekt>::iterator o = file->bounding_volumes.begin(); o != file->bounding_volumes.end(); ++o) {
				t = lv.intersect(*o);
				if (0.0 < t) {
					volumes_hit.push_back(o->getSurface()->getNumber());
				}
			}
			for (std::vector<Objekt>::iterator o = file->objekte.begin(); o != file->objekte.end(); ++o) {
				for (std::vector<int>::iterator s = volumes_hit.begin(); s != volumes_hit.end(); ++s) {
					if (o->getSurface()->getNumber() == *s){
						t = lv.intersect(*o);
						if (t > 0.0) {
							something_intersected = true;
							break;
						}
					}
				}
			}
			if (something_intersected == false) {
				Color new_color = shaded_color(&(*li), reflected_ray, normal, closest);
				cur_color = cur_color.addcolor(new_color);
			} 
		}

		if (depth < this->maxdepth) {
			Color mirror_color = reflected_ray.shade(file);
			mirror_color = mirror_color.scmpy(closest->getProperty()->getMirror());
			cur_color = mirror_color.addcolor(cur_color);
		}
	}
	return(cur_color);
} /* shade() */


/*----------------------------------------------------------------------------*/
/* shaded_color   berechnet die Farbe im Treffpunkt, die durch die           */
/*   Lichtquelle hervorgerufen wird.                                          */
/* Aufrufparameter:    Lichtquelle                                            */
/*                     Reflexionsstrahl                                       */
/*                     Normalenvektor im Treffpunkt                           */
/*                     aktuelles Objekt                                       */
/* Rueckgabeparameter: errechnete Farbe                                       */
/*----------------------------------------------------------------------------*/

Color Ray::shaded_color(Light *light, Ray &reflectedray, Vector &normal, Objekt *obj)
{
	Color reflected_color;
	Color specular;
	Color lambert;
	double spec;

	double ldot;
	ldot = light->getDirection().dot(normal);
	reflected_color = black;
	if (1.0 + ldot > 1.0) {
		lambert = light->getColor().scmpy(ldot);
		reflected_color = lambert.outprodc(obj->getProperty()->getReflectance());
	}
	spec = reflectedray.getDirection().dot(light->getDirection());

	if (1.0 + spec > 1.0) {

		spec = spec * spec;
		spec = spec * spec;
		spec = spec * spec;
		spec *= obj->getProperty()->getSpecular();
		spec *= std::pow(std::acos(ldot),obj->getProperty()->getShininess());// K-Value
		specular =  light->getColor().scmpy(spec);
		reflected_color = reflected_color.addcolor(specular);
	}

	return reflected_color;
} /* shaded_color() */

/*----------------------------------------------------------------------------*/
/* reflect   berechnet einen Reflektionsstrahl aus dem einfallenden Strahl an */
/*    dem Reflektionspunkt mit Hilfe des Normalenvektors an dieser Stelle.    */
/* Aufrufparameter:    einfallender Strahl                                    */
/*                     Reflektionspunkt                                       */
/*                     Normalenvektor                                         */
/* Rueckgabeparameter: Reflektionsstrahl                                      */
/*----------------------------------------------------------------------------*/

Ray Ray::reflect(Vector &origin, Vector &normal)
{
	Ray	 reflection;
	double	 incdot;

	incdot = normal.dot(direction);
	reflection.origin = origin;
	reflection.direction = normal.svmpy(2.0*incdot);
	reflection.direction = direction.vsub(reflection.direction).normalize();

	reflection.depth = depth + 1;
	reflection.maxdepth = maxdepth;
	return(reflection);
} /* reflect() */


double Ray::intersectQuadric(Objekt &obj){
	Surface* surface = obj.getSurface();
	double a, b, c, d, e, f, g, h, j, k, t = -1.0,
		acoef, bcoef, ccoef, root, disc;

	a = surface->a; 
	b = surface->b; 
	c = surface->c;
	d = surface->d; 
	e = surface->e; 
	f = surface->f;
	g = surface->g; 
	h = surface->h; 
	j = surface->j;
	k = surface->k;

	acoef = Vector(direction.dot(Vector(a, b, c)),
		e*direction.y + f*direction.z,
		h*direction.z).dot(direction);

	bcoef = 
		Vector(d, g, j).dot(direction) + 
		origin.dot( Vector( 
		direction.dot(Vector(a+a , b, c)), 
		direction.dot(Vector(b, e+e, f)), 
		direction.dot(Vector(c, f, h+h))));

	ccoef = origin.dot( 
		Vector(Vector(a, b, c).dot(origin) + d,
		e*origin.y + f*origin.z + g,
		h*origin.z + j)) + k;

	if (1.0 + acoef != 1.0) {
		disc = bcoef * bcoef - 4 * acoef * ccoef;
		if (disc > -DBL_EPSILON) {
			root = sqrt( disc );
			t = ( -bcoef - root ) / ( acoef + acoef );
			if (t < 0.0) {
				t = ( -bcoef + root ) / ( acoef + acoef );
			}
		}
	}
	return ((0.001 < t) ? t : -1.0);
} /* intersect() */

double Ray::intersect(Objekt &obj){
	if (obj.getSurface()->getType() == surface_type::QUADRIC)
		return intersectQuadric(obj);
	else if (obj.getSurface()->getType() == surface_type::POLYGON){
		return intersectPolygon(obj);
	}
}

double Ray::intersectPolygon(Objekt &obj){
	Surface* polygon = obj.getSurface();
	Vector u = polygon->vertex_index[1].vsub(polygon->vertex_index[0]);		// V1->V2
	Vector v = polygon->vertex_index[2].vsub(polygon->vertex_index[0]);		// V1->V3
	Vector w = u.cross(v);
	Vector w0;
	
	if (w.x == 0 && w.y == 0 && w.z == 0)
		return -1.0;	// triangle is degenerate -> not dealing with this case

	Vector rPos = this->getOrigin();
	Vector rDir = this->getDirection();
	w0 = rPos.vsub(polygon->vertex_index[0]);		// Ray -> V1

	float first = -w.dot(w0);
	float second = w.dot(rDir);
	if (fabs(second) < 0.000001)	// ray parallel to triangle plane
		if (first == 0)				// ray disjoint from plane
			return -1.0;			// ray lies triangle plane
		else
			return -1.0;			// ray disjoint from plane

	float r = first/second;			// distance to interPoint
	if (r < 0.0 || second == 0 )
		return -1.0;
	
	
	Vector interPoint = this->getDirection().svmpy(r).vadd(this->getOrigin());	//calc interPoint
	Vector normal = w;	// normale beim erstellen berechnen -> averaged normals

	float uu,uv,vv,wu,wv,div;

	uu = u.dot(u);
	uv = u.dot(v);
	vv = v.dot(v);
	w = interPoint.vsub(polygon->vertex_index[0]);		// V1 -> interPoint
	wu = w.dot(u);
	wv = w.dot(v);
	div = (uv * uv - uu * vv);

	float s = (uv * wv - vv * wu) / div;				// V1 * 
	if (s < 0.0 || s > 1.0)
		return -1.0;
	float t = (uv * wu - uu * wv) / div;
	if (t < 0.0 || (s+t) > 1.0)
		return -1.0;

	float y = 1.0f - s - t;

	// texture coordinates, still testing!
	//float ax = (this->texb.r - this->texa.r) * s;
	//float ay = (this->texc.g - this->texa.g) * t;
	//local.setTexCoords(vec2(ax,ay));

	Vector normal1 = polygon->average_normals[0]->normalize().svmpy(y);
	Vector normal2 = polygon->average_normals[1]->normalize().svmpy(s);
	Vector normal3 = polygon->average_normals[2]->normalize().svmpy(t);
	normal = normal1.vadd(normal2).vadd(normal3).normalize();

	obj.set_normal(normal);
	
	return ((0.001 < r) ? r : -1.0);
}
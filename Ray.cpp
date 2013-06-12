#include "StdAfx.h"
#include "Ray.h"
#include "math.h"
#include "float.h"
#include <vector>


using namespace std;

extern Color background, ambience;
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

Color Ray::shade(vector<Objekt> &objects, vector<Light> &lights, Color* background, Color* ambience)
{
	Objekt *closest = NULL;
	Color cur_color; 
	double min_t = DBL_MAX, t;

	Vector intersection_position,	normal;
	Ray lv, reflected_ray;
	bool something_intersected = false;

	for (vector<Objekt>::iterator o = objects.begin(); o != objects.end(); ++o) {

		t = intersect(*o);
		if (0.0 < t && t < min_t) {
			min_t = t;
			closest = &(*o);
		}
	}

	if (closest == NULL) {
		if (depth == 0)
			cur_color = *background; //background_color;
		else
			cur_color = black;
	} else {
		intersection_position = origin.vadd(direction.svmpy(min_t));
		normal = closest->get_normal(intersection_position);
		reflected_ray = reflect(intersection_position, normal);
		cur_color = closest->getProperty()->getAmbient().outprodc(*ambience);  // black statt Globales Ambient -> nun ambient

		for (vector<Light>::iterator li = lights.begin(); li != lights.end(); ++li) {
			lv.setDirection(li->getDirection());
			lv.setOrigin(intersection_position);
			something_intersected = false;
			for (vector<Objekt>::iterator o = objects.begin(); o != objects.end(); ++o) {
				t = lv.intersect(*o);
				if (t > 0.0) {
					something_intersected = true;
					break;
				}
			}
			if (something_intersected == false) {
				Color new_color = shaded_color(&(*li), reflected_ray, normal, closest);
				cur_color = cur_color.addcolor(new_color);
			} 
		}

		if (depth < this->maxdepth) {
			Color mirror_color = reflected_ray.shade(objects, lights, background, ambience);
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
		spec *= std::pow(std::acos(ldot),obj->getProperty()->getShininess());
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
	Vector u = polygon->v2.vsub(polygon->v1);
	Vector v = polygon->v3.vsub(polygon->v1);
	Vector w = u.cross(v);
	Vector w0;
	
	if (w.x == 0 && w.y == 0 && w.z == 0)
		return -1.0;	// triangle is degenerate -> not dealing with this case

	Vector rPos = this->getOrigin();
	Vector rDir = this->getDirection();
	w0 = rPos.vsub(polygon->v1);

	float first = -w.dot(w0);
	float second = w.dot(rDir);
	if (fabs(second) < 0.000001) // ray parallel to triangle plane
		if (first == 0)     // ray disjoint from plane
			return -1.0;   // ray lies triangle plane
		else
			return -1.0;	// ray disjoint from plane

	float r = first/second;
	if (r < 0.0 || second == 0 )
		return -1.0;
	
	
	Vector interPoint = this->getDirection().svmpy(r).vadd(this->getOrigin());
	//Vector interPoint = this->getOrigin().vadd(this->getDirection()).svmpy(r);
	Vector normal = w;

	float uu,uv,vv,wu,wv,div;

	uu = u.dot(u);
	uv = u.dot(v);
	vv = v.dot(v);
	w = interPoint.vsub(polygon->v1);
	wu = w.dot(u);
	wv = w.dot(v);
	div = (uv * uv - uu * vv);

	float s = (uv * wv - vv * wu) / div;
	if (s < 0.0 || s > 1.0)
		return -1.0;
	float t = (uv * wu - uu * wv) / div;
	if (t < 0.0 || (s+t) > 1.0)
		return -1.0;

	// texture coordinates, still testing!
	//float ax = (this->texb.r - this->texa.r) * s;
	//float ay = (this->texc.g - this->texa.g) * t;
	//local.setTexCoords(vec2(ax,ay));

	//local.setPos(interPoint);
	obj.set_normal(normal);
	//thit = r;
	return ((0.001 < r) ? r : -1.0);
}
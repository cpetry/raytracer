#include "StdAfx.h"
#include "Objekt.h"
#include "Surface.h"

/*----------------------------------------------------------------------------*/
/* get_normal   gibt den Normalenvektor der Oberflaeche eines Objekts an dem  */
/*   in 'v' bestimmten Punkt zurueck.                                         */
/* Aufrufparameter:    Punkt-Vektor, fuer den der N.-Vektor berechnet werd. s.*/
/*                     Datenstruktur in der der Objektpointer enthalten ist   */
/* Rueckgabeparameter: Berechneter Normalenvektor                             */
/*----------------------------------------------------------------------------*/

Vector Objekt::get_normal(Vector &v)
{
	Vector normal;

	if (surface->getType() == surface_type::QUADRIC){
		normal = Vector( 
			v.dot(Vector(surface->a+surface->a, surface->b, surface->c)) + surface->d,
			v.dot(Vector(surface->b, surface->e+surface->e, surface->f)) + surface->g,
			v.dot(Vector(surface->c, surface->f, surface->h+surface->h)) + surface->j);
	}
	
	else if (surface->getType() == surface_type::POLYGON){
		normal = this->normal;
		
		/*
		Vector u = surface->v2.vsub(surface->v1);
        Vector v = surface->v3.vsub(surface->v1);
 
        normal = Vector((u.y * v.z - u.z * v.y),
						(u.z * v.x - u.x * v.z),
						(u.x * v.y - u.y * v.x));*/
	}

	return normal.normalize();
} /* get_normal() */


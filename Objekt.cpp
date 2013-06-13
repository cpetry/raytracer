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

Vector Objekt::get_normal(Vector &v, bool gouraud_shaded)
{
	Vector normal;

	if (surface->getType() == surface_type::QUADRIC){
		normal = Vector( 
			v.dot(Vector(surface->a+surface->a, surface->b, surface->c)) + surface->d,
			v.dot(Vector(surface->b, surface->e+surface->e, surface->f)) + surface->g,
			v.dot(Vector(surface->c, surface->f, surface->h+surface->h)) + surface->j);
	}
	
	else if (surface->getType() == surface_type::POLYGON){
		if (!gouraud_shaded){
			Vector u = surface->vertex_index[1].vsub(surface->vertex_index[0]);		// V1->V2
			Vector v = surface->vertex_index[2].vsub(surface->vertex_index[0]);		// V1->V3
			Vector w = u.cross(v);
			return w.normalize();
		}
		
		normal = this->normal;
	}

	return normal.normalize();
} /* get_normal() */


#include "RenderThread.h"

#include <chrono>

#include "Ray.h"

//const double SCREENWIDTH = 1000;
//const double SCREENHEIGHT = 1000;

void RenderThread::renderPicture(){
	// Setting up start time
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::milliseconds milliseconds;
	clock::time_point start_time = clock::now();
	int refresh_percentage=0;

	// Setting up coordinate system
	int Xresolution = img->file->resolutionX;
	int Yresolution = img->file->resolutionY;
	Vector u,v,w;
	w = img->file->eye.vsub(Vector(0,0,0)).normalize();
	u = img->file->up.cross(w).normalize();
	v = w.cross(u).normalize();
	float fovx = img->file->fovy * img->file->aspect / 180 * 3.1415926;
		
	// Settings for Super Sampling
	float coef = 1.0f;
	float steps = 1.0f;
	if (img->file->super_sampling != 0){	
		coef = 1.0f / img->file->super_sampling;
		steps = 1.0f / std::sqrt(img->file->super_sampling);
	}


	for (int scanline=0; scanline < Yresolution; scanline++) {
		for (int sx=0; sx < Xresolution; sx++) {
			
			Color color(0,0,0);
			
			// SuperSampling
			for(float fragmentx = sx; fragmentx < sx + 1.0f; fragmentx += steps)
			for(float fragmenty = scanline; fragmenty < scanline + 1.0f; fragmenty += steps)
			{
				// Each ray contribute to the quarter of a full pixel contribution.
				float alpha = std::tanf (fovx / 2.0f) * ((fragmentx - (Xresolution / 2.0f))  / (Xresolution  / 2.0f));
				float beta  = std::tanf (img->file->fovy / 180 * 3.1415926 / 2.0f) * (((Yresolution  / 2.0f) - fragmenty) / (Yresolution / 2.0f));
				Ray	ray(u.svmpy(alpha).vadd(v.svmpy(beta)).vsub(w).normalize(), img->file->eye, 0, img->file->bounces);

				color = color.addcolor(ray.shade(img->file).scmpy(coef));
			}
			
			this->img->set(sx, scanline,
				color.r > 1.0 ? 255 : int(255 * color.r),
				color.g > 1.0 ? 255 : int(255 * color.g),
				color.b > 1.0 ? 255 : int(255 * color.b));
		}

		// calculating percentage and updating picture
		int percentage = scanline * 1.0f / Yresolution * 1.0f * 100;
		if (percentage > refresh_percentage || scanline == Yresolution-1){
			refresh_percentage = percentage + this->update_precentage - 1;
			clock::time_point spent_time = clock::now();
			milliseconds total_ms = std::chrono::duration_cast<milliseconds>(spent_time - start_time);
			this->time_spent = total_ms.count() / 1000.f;
			emit resultReady(img, refresh_percentage, time_spent);
		}

		//printf("%4d\r", Yresolution-scanline);
	}
}
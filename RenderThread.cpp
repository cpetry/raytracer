#include "RenderThread.h"
#include "Ray.h"

const double SCREENWIDTH = 1000;
const double SCREENHEIGHT = 1000;

void RenderThread::renderPicture(float update_percentage){

	//resolutionX = 1250;
	//resolutionY = 1250;
	int Xresolution = img->file->resolutionX;
	int Yresolution = img->file->resolutionY;
	double dx = SCREENWIDTH / (double)Xresolution;
	double dy = SCREENHEIGHT / (double)Yresolution;
	double y = -0.5 * SCREENHEIGHT;
	//Vector eye(0, 0, SCREENHEIGHT * 8.0);
	Ray	ray(img->file->lookat, img->file->eye ,0);

	int refresh_percentage=0;

	for (int scanline=0; scanline < Yresolution; scanline++) {
		int percentage = scanline * 1.0f / Yresolution * 1.0f;
		if (percentage > refresh_percentage){
			refresh_percentage = percentage + update_percentage;

		}

		printf("%4d\r", Yresolution-scanline);
		y += dy;
		double x = -0.5 * SCREENWIDTH;

		for (int sx=0; sx < Xresolution; sx++) {
			ray.setDirection(Vector(x, y, 0.0).vsub(ray.getOrigin()).normalize());
			x += dx;
			Color color = ray.shade(img->file->objekte, img->file->lights);

			this->img->set(sx, scanline,
				color.r > 1.0 ? 255 : int(255 * color.r),
				color.g > 1.0 ? 255 : int(255 * color.g),
				color.b > 1.0 ? 255 : int(255 * color.b));
		}
	}
}
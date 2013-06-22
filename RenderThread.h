#pragma once

#include <Qthread>

#include "Image.h"

class RenderThread : public QThread
{
    Q_OBJECT
public:
	RenderThread(){};
	RenderThread(Image* img, int update_precentage){
		this->img = new Image(img->file->resolutionX, img->file->resolutionY, img->file);
		*this->img = *img;
		this->update_precentage = update_precentage;
		this->time_spent = 0;
	}
    void run() Q_DECL_OVERRIDE {
        /* expensive or blocking operation  */
		this->renderPicture();
    }
private:
	int update_precentage;
	float time_spent;
	Image* img;
	void renderPicture();

signals:
    void resultReady(Image* pic, int percentage, float time_spent);
};
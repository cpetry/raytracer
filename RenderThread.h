#include "Qthread"
#include "Image.h"

class RenderThread : public QThread
{
    Q_OBJECT
public:
	RenderThread(Image* img){
		this->img = new Image(img->file->resolutionX, img->file->resolutionY, img->file);
		*this->img = *img;
	}
    void run() Q_DECL_OVERRIDE {
        /* expensive or blocking operation  */
		this->renderPicture(/*refresh %*/ 5.0f);

        emit resultReady(img);
    }
private:
	Image* img;
	void RenderThread::renderPicture(float update_percentage);
signals:
    void resultReady(Image* pic);
};
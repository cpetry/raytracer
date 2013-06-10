#pragma once

#include <QtWidgets/qmainwindow>
#include <QtWidgets\qfiledialog>
#include "ui_GUI.h"

class File;
class Image;

class GUI : public QMainWindow
{
	Q_OBJECT

public:
	File* file;
	QImage* img;
	bool isRendering;

	GUI(QWidget *parent = 0);
	~GUI(){};

	void init();
	void closeEvent(QCloseEvent *event);
	void setGui(QString filename);
	

//slots
public slots:
	void slot_updatePicture(Image* pic, int percentage, float time_spent);
	void slot_ButtonRender();
	void slot_ButtonSearch();
	void slot_ChooseBackground();
	void slot_ChooseAmbience();
	void slot_ChooseSuperSampling(const QString & text);
	void slot_ChooseAspect(const QString & text);
	void slot_SaveImage();
	void slot_isRendering();

private:
	Ui::MainWindow ui;
	void setFile();
};
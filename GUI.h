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

	GUI(QWidget *parent = 0);
	~GUI(){};

	void init();
	void closeEvent(QCloseEvent *event);
	void setGui(QString filename);
	

//slots
public slots:
	void slot_updatePicture(Image* pic);
	void slot_ButtonRender(const QVariant &);
	void slot_ButtonSearch(const QVariant &);
	void slot_ChooseBackground(const QVariant &);

    // note: just a sample slot demonstrating a signal from the backend thread
    void new_image() {
        // TODO: update gui
        printf(">>> New Image arrived! <<<\n\n");
    }

//signals:
    //void stop_backend_thread();

private:
	Ui::MainWindow ui;
	void setFile();
};
#include <string>

#include "GUI.h"

#include <sstream>

#include <Qt3D/qglview.h>
#include "Qt3D/qglbuilder.h"
#include "qgraphicsview.h"
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QAction>
#include <Qgridlayout>
#include <QCloseEvent>
#include <QTextBrowser>
#include <QScreen>
#include <QColorDialog>
#include <Qbrush>
#include <QComboBox>
#include <QMenuBar>
#include <QStatusBar>
#include <qcheckbox.h>

#include "File.h"
#include "Image.h"
#include "RenderThread.h"
#include <qsizepolicy>

GUI::GUI(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	this->init();
}

void GUI::init(){
	this->isRendering = false;
	this->setWindowTitle("Raytracer");
	//setGui("data/bunny_scaled.data");
	//setGui("data/scene.data");
	
	// setting update percentage
	QLineEdit* line_edit_updatePercentage = this->centralWidget()->findChild<QLineEdit *>("lineEditUpdatePercentage");
	if (line_edit_updatePercentage)
		line_edit_updatePercentage->setText(QString::number(25));

	// setting SuperSampling
	QComboBox* combobox_supersampling = this->centralWidget()->findChild<QComboBox *>("comboBoxSuperSampling");
	if (combobox_supersampling){
		combobox_supersampling->addItem(QString("0"));
		combobox_supersampling->addItem(QString("4"));
		combobox_supersampling->addItem(QString("16"));
	}

	// setting Aspect
	QComboBox* combobox_aspect = this->centralWidget()->findChild<QComboBox *>("comboBoxAspect");
	if (combobox_aspect){
		combobox_aspect->addItem(QString("1:1"));
		combobox_aspect->addItem(QString("4:3"));
		combobox_aspect->addItem(QString("16:9"));
		combobox_aspect->addItem(QString("16:10"));
		combobox_aspect->setCurrentIndex(-1);
	}
	
	// Connecting Signals and Slots
	connect(this->findChild<QPushButton *>("buttonRender"),		&QPushButton::clicked,			this, &GUI::slot_ButtonRender);
	connect(this->findChild<QPushButton *>("buttonSearch"),		&QPushButton::clicked,			this, &GUI::slot_ButtonSearch);
	connect(this->findChild<QPushButton *>("buttonLoad"),		&QPushButton::clicked,			this, &GUI::slot_ButtonLoad);
	connect(this->findChild<QToolButton *>("buttonBackground"), &QToolButton::clicked,			this, &GUI::slot_ChooseBackground);
	connect(this->findChild<QToolButton *>("buttonAmbience"),	&QToolButton::clicked,			this, &GUI::slot_ChooseAmbience);
	connect(this->findChild<QCheckBox *>("checkGouraud"),		&QCheckBox::stateChanged,		this, &GUI::slot_CheckGouraud);
	connect(this->findChild<QLineEdit *>("lineEditAspect"),		&QLineEdit::textEdited,			this, &GUI::slot_EditedTextAspect);
	connect(combobox_supersampling,								&QComboBox::currentTextChanged,	this, &GUI::slot_ChooseSuperSampling);
	connect(combobox_aspect,									&QComboBox::currentTextChanged,	this, &GUI::slot_ChooseAspect);
}

/*
 * When a File is loaded -> setting up GUI
 */
void GUI::setGui(QString filename){
	QWidget* central = this->centralWidget();

	central->findChild<QLineEdit *>("lineEditFilename")->setText(filename);

	// opening file
	this->file = new File();
	file->openFile(filename);
		
	// setting resolution
	if (file->resolutionX > 0 && file->resolutionY > 0){
		central->findChild<QLineEdit *>("lineEditResX")->setText(QString::number(file->resolutionX));
		central->findChild<QLineEdit *>("lineEditResY")->setText(QString::number(file->resolutionY));
	}

	// setting FOV
	central->findChild<QLineEdit *>("lineEditFOV")->setText(QString::number(file->fovy));

	// setting aspect
	central->findChild<QLineEdit *>("lineEditAspect")->setText(QString::number(file->aspect));
	
	// setting bounces
	central->findChild<QLineEdit *>("lineEditBounces")->setText(QString::number(file->bounces));

	// setting ambience and background color
	std::stringstream color_background, color_ambience;
	color_background << "background-color: rgb(" << file->background.r*255 << "," << file->background.g*255 << "," << file->background.b*255 << ")";
	central->findChild<QFrame *>("frameBackground")->setStyleSheet(color_background.str().c_str());

	color_ambience << "background-color: rgb(" << file->ambience.r*255 << "," << file->ambience.g*255 << "," << file->ambience.b*255 << ")";
	central->findChild<QFrame *>("frameAmbience")->setStyleSheet(color_ambience.str().c_str());
	
}

/* 
 * when changing parameters for raytracing -> change intern file settings
 */
void GUI::setFile(){
	file->resolutionX = this->centralWidget()->findChild<QLineEdit *>("lineEditResX")->text().toInt();
	file->resolutionY = this->centralWidget()->findChild<QLineEdit *>("lineEditResY")->text().toInt();
	file->bounces = this->centralWidget()->findChild<QLineEdit *>("lineEditBounces")->text().toInt();
	file->fovy = this->centralWidget()->findChild<QLineEdit *>("lineEditFOV")->text().toDouble();
	file->aspect = this->centralWidget()->findChild<QLineEdit *>("lineEditAspect")->text().toDouble();
}


//////////
//Slots
//////////

void GUI::slot_ButtonRender(){
	FILE *yyin;
	yyin = fopen(this->findChild<QLineEdit *>("lineEditFilename")->text().toUtf8().data(),"r");
	if(yyin == NULL || this->file == NULL) {
		return;
	}

	if (this != NULL && this->file != NULL && !this->isRendering){
		setFile(); // setting intern file to current settings

		Image* bild = new Image(this->file->resolutionX, this->file->resolutionY, this->file);

		int update_precentage = this->centralWidget()->findChild<QLineEdit *>("lineEditUpdatePercentage")->text().toInt();

		renderThread = new RenderThread(bild, update_precentage);
		connect(renderThread, &RenderThread::resultReady, this, &GUI::slot_updatePicture);
		connect(renderThread, SIGNAL(finished()), renderThread, SLOT(deleteLater()));
		connect(renderThread, SIGNAL(finished()), this, SLOT(slot_isRendering()));

		this->findChild<QPushButton *>("buttonRender")->setText("Stop");
		this->isRendering = true;
		renderThread->start();
		renderThread->setPriority(QThread::Priority::TimeCriticalPriority);
	}
	else if (renderThread != NULL){
		renderThread->quit();
		this->findChild<QPushButton *>("buttonRender")->setText("Render");
	}
}

void GUI::slot_ButtonSearch(){
	QString selfilter = tr("files (*.data *.obj)");
	QString fileName = QFileDialog::getOpenFileName(
        this,
        "open render-file",
        NULL,
        tr("files (*.data *.obj)" ),
        &selfilter 
		);

	if (fileName != "")
		this->findChild<QLineEdit *>("lineEditFilename")->setText(fileName);
}

void GUI::slot_ButtonLoad(){
	setGui(this->centralWidget()->findChild<QLineEdit *>("lineEditFilename")->text());
}

void GUI::slot_isRendering(){
	this->isRendering = false;
	this->findChild<QPushButton *>("buttonRender")->setText("Render");
}

void GUI::slot_ChooseBackground(){
	QColor c = QColorDialog::getColor(Qt::white, this);	// gets color by letting user choose
	this->file->background = Color(c.red()/255.0f,c.green()/255.0f,c.blue()/255.0f);

	std::stringstream color_background;
	color_background << "background-color: rgb(" << file->background.r*255 << "," << file->background.g*255 << "," << file->background.b*255 << ")";
	centralWidget()->findChild<QFrame *>("frameBackground")->setStyleSheet(color_background.str().c_str());
}

void GUI::slot_ChooseAmbience(){
	QColor c = QColorDialog::getColor(Qt::white, this);
	this->file->ambience = Color(c.red()/255.0f,c.green()/255.0f,c.blue()/255.0f);

	std::stringstream color_ambience;
	color_ambience << "background-color: rgb(" << file->ambience.r*255 << "," << file->ambience.g*255 << "," << file->ambience.b*255 << ")";
	centralWidget()->findChild<QFrame *>("frameAmbience")->setStyleSheet(color_ambience.str().c_str());
}

void GUI::slot_ChooseSuperSampling(const QString & text){
	file->super_sampling = text.toInt();
}

void GUI::slot_ChooseAspect(const QString & text){
	float aspect = 1;
	if (text == "1:1")
		aspect = 1;
	else if (text == "4:3")
		aspect = 4.0f/3.0f;
	else if (text == "16:9")
		aspect = 16.0f/9.0f;
	else if (text == "16:10")
		aspect = 16.0f/10.0f;

	file->aspect = aspect;
	this->centralWidget()->findChild<QLineEdit *>("lineEditAspect")->setText(QString::number(aspect));
}

void GUI::slot_CheckGouraud(int state){
	if (state == Qt::Unchecked){
		file->gouraud_shaded = false;
	}
	else
		file->gouraud_shaded = true;
}

void GUI::slot_EditedTextAspect(const QString & text){
	file->aspect = text.toDouble();
	this->centralWidget()->findChild<QComboBox *>("comboBoxAspect")->setCurrentIndex(-1);
}

void GUI::slot_updatePicture(Image* pic, int percentage, float time_spent){

	// Setting Image
	QImage image = QImage(this->file->resolutionX, this->file->resolutionY, QImage::Format::Format_RGB888);
	for(int y = 0; y < this->file->resolutionY; y++) 
		for(int x = 0; x < this->file->resolutionX; x++) 
			image.setPixel(x, y, qRgb(pic->getR(x,y), pic->getG(x,y), pic->getB(x,y)));

	this->img = new QImage(image);

	QGraphicsPixmapItem* item = new QGraphicsPixmapItem( QPixmap::fromImage(image));

	// finding and updating render window
	QWidget* central = this->centralWidget();
	QMainWindow* graphics_view = central->findChild<QMainWindow *>("Rendered picture");
	QSizePolicy exp;
	
	if (graphics_view){
		//graphics_view->setVisible(true);
		graphics_view->setVisible(true);
		QGraphicsView* graphics_widget = static_cast<QGraphicsView*> (graphics_view->centralWidget());
		graphics_widget->scene()->clear();
		graphics_widget->scene()->addItem(item);
		graphics_widget->setFixedSize(image.width(), image.height());
		graphics_view->setFixedSize(graphics_widget->width(), 
									graphics_widget->height()
									+ graphics_view->statusBar()->size().height()
									+ graphics_view->menuBar()->size().height());
	}
	else{
		graphics_view = new QMainWindow(central);
		central->activateWindow();
		graphics_view->setVisible(true);
		graphics_view->move(QPoint(0,0));
		graphics_view->setStatusBar(new QStatusBar());
		graphics_view->setObjectName("Rendered picture");
		graphics_view->setWindowTitle("Rendered picture");
		graphics_view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);	// to resize window 
		
		// create MenuBar and connect signals
		QMenuBar* menu  = new QMenuBar();
		menu->addMenu("File");
		QMenu* menu_file = menu->findChild<QMenu *>();
		QAction* menu_save = menu_file->addAction("Save as..");
		connect (menu_save, &QAction::triggered, this, &GUI::slot_SaveImage);
		graphics_view->setMenuBar(menu);

		
		QGraphicsView* graphics_widget = new QGraphicsView(graphics_view);
		graphics_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphics_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		
		graphics_view->setCentralWidget(graphics_widget);
		QGraphicsScene* scene = new QGraphicsScene;
		scene->addItem(item);
		graphics_widget->setScene(scene);
		graphics_view->show();
	}
	
	graphics_view->statusBar()->showMessage(QString::number(time_spent) + QString("secs rendered"));
	central->findChild<QProgressBar *>("progressBar")->setValue(percentage);
}

void GUI::slot_SaveImage(){
	QString selfilter = tr("PNG (*.png)");
	QString fileName = QFileDialog::getSaveFileName(
	    this,
		"Save rendered Image",
		NULL,
		tr("PNG (*.png);; PPM (*.ppm);; JPG (*.jpg);; BMP (*.bmp)"),
		&selfilter 
	);
	if (fileName != ""){
		this->img->save(fileName.toUtf8());
	}
}

// override
void GUI::closeEvent(QCloseEvent *event){
	auto reply = QMessageBox::question(this, "Quit?", "You really want to quit?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);
	if (reply == QMessageBox::StandardButton::Yes) {
        //emit stop_backend_thread();
		event->accept();
    }
	else
		event->ignore();
}
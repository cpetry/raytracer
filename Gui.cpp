#include <string>

#include "GUI.h"

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

#include "File.h"
#include "Image.h"
#include "RenderThread.h"
#include <qsizepolicy>

GUI::GUI(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	this->init();
	QPushButton* button_render = this->findChild<QPushButton *>("buttonRender");
	QPushButton* button_search = this->findChild<QPushButton *>("buttonSearch");
	QPushButton* choose_background = this->findChild<QPushButton *>("chooseBackground");

	connect(button_render, &QPushButton::clicked, this, &GUI::slot_ButtonRender);
	connect(button_search, &QPushButton::clicked, this, &GUI::slot_ButtonSearch);
	connect(choose_background, &QPushButton::clicked, this, &GUI::slot_ChooseBackground);
}

void GUI::init(){
	this->setWindowTitle("Raytracer");
}

void GUI::setGui(QString filename){
	QWidget* central = this->centralWidget();

	// setting filename
	QLineEdit* textbrowser = central->findChild<QLineEdit *>("lineEditFilename");
	if (textbrowser)
		textbrowser->setText(filename);

	// setting resolution
	this->file = new File();
	file->openFile(filename);
	QLineEdit* line_edit_resx = central->findChild<QLineEdit *>("lineEditResX");
	QLineEdit* line_edit_resy = central->findChild<QLineEdit *>("lineEditResY");
	if (line_edit_resx && line_edit_resy && file->resolutionX > 0 && file->resolutionY > 0){
		line_edit_resx->setText(QString::number(file->resolutionX));
		line_edit_resy->setText(QString::number(file->resolutionY));
	}

	// setting ambience and background color
	QPushButton* choose_ambience = central->findChild<QPushButton *>("frameAmbience");
	QPushButton* choose_background = central->findChild<QPushButton *>("frameBackground");
	if (choose_ambience && choose_background){
		QBrush brush = choose_ambience->palette().button();
		brush.setColor(QColor(file->ambience.r, file->ambience.g, file->ambience.b));
		QPalette p = choose_ambience->palette();
		p.setBrush(QPalette::Button, brush);
		choose_ambience->setPalette(p);
		
		brush = choose_background->palette().button();
		brush.setColor(QColor(file->background.r, file->background.g, file->background.b));
		p = choose_background->palette();
		p.setBrush(QPalette::Button, brush);
		choose_background->setPalette(p);
	}

}

// when changing parameters for raytracing -> change intern file settings
void GUI::setFile(){
	QWidget* central = this->centralWidget();
	QLineEdit* line_edit_resx = central->findChild<QLineEdit *>("lineEditResX");
	QLineEdit* line_edit_resy = central->findChild<QLineEdit *>("lineEditResY");
	if (line_edit_resx && line_edit_resy){
		file->resolutionX = line_edit_resx->text().toInt();
		file->resolutionY = line_edit_resy->text().toInt();
	}
}

void GUI::slot_updatePicture(Image* pic){

	// Setting Image
	QImage image = QImage(this->file->resolutionX, this->file->resolutionY, QImage::Format::Format_RGB888);
	for(int y = 0; y < this->file->resolutionY; y++) 
		for(int x = 0; x < this->file->resolutionX; x++) 
			image.setPixel(x, y, qRgb(pic->getR(x,y), pic->getG(x,y), pic->getB(x,y)));

	QGraphicsPixmapItem* item = new QGraphicsPixmapItem( QPixmap::fromImage(image));

	// finding and updating render window
	QWidget* central = this->centralWidget();
	QMainWindow* graphics_view = central->findChild<QMainWindow *>("Rendered picture");
	QSizePolicy exp;
	
	if (graphics_view){
		graphics_view->setFixedSize(image.width(), image.height());

		QGraphicsView* graphics_widget = static_cast<QGraphicsView*> (graphics_view->centralWidget());
		graphics_widget->scene()->clear();
		graphics_widget->scene()->addItem(item);
		
		
	}
	else{
		graphics_view = new QMainWindow(central);
		graphics_view->setObjectName("Rendered picture");
		graphics_view->setWindowTitle("Rendered picture");
		graphics_view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		QGraphicsView* graphics_widget = new QGraphicsView();
		graphics_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphics_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		
		graphics_view->setCentralWidget(graphics_widget);
		QGraphicsScene* scene = new QGraphicsScene;
		scene->addItem(item);
		graphics_widget->setScene(scene);
		
		graphics_view->show();
	}	
}


//////////
//Slots
//////////


void GUI::slot_ButtonRender(const QVariant &){
	setFile();

	if (this->file != NULL){
		Image* bild = new Image(this->file->resolutionX, this->file->resolutionY, this->file);
		RenderThread* render_thread = new RenderThread(bild);
		connect(render_thread, &RenderThread::resultReady, this, &GUI::slot_updatePicture);
		connect(render_thread, &RenderThread::finished, render_thread, &QObject::deleteLater);
		render_thread->start();
	}
}

void GUI::slot_ButtonSearch(const QVariant &){
	QString selfilter = tr("files (*.data *.obj)");
	QString fileName = QFileDialog::getOpenFileName(
        this,
        "open render-file",
        NULL,
        tr("files (*.data *.obj)" ),
        &selfilter 
		);

	if (fileName != "")
		this->setGui(fileName);
}

void GUI::slot_ChooseBackground(const QVariant &){
	QColor c = QColorDialog::getColor(Qt::white, this);
	QPushButton* choose_background = this->centralWidget()->findChild<QPushButton *>("chooseBackground");
	if (choose_background){
		this->file->background = Color(c.red(),c.green(),c.blue());
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
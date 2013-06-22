// main.cpp : Defines the entry point for the console application.
//
#include <qmainwindow>
#include <qapplication>

#include "GUI.h"


int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	GUI gui;
	gui.show();

	return app.exec();
}


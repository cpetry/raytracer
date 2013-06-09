// main.cpp : Defines the entry point for the console application.
//
#include <qmainwindow>
#include <qapplication>

#include <stdio.h>

#include "GUI.h"


int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	GUI gui;
	gui.show();


	//Image* bild = File("data/dflt.data");
	//gui.setFile("data/dflt.data");

	/*
	std::string name = "raytrace-bild.ppm";
	bild->save(name.c_str());*/

	return app.exec();
}


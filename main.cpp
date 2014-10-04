#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setOrganizationName("Bilkon");
	QCoreApplication::setOrganizationDomain("bilkon-kontrol.com");
	QCoreApplication::setApplicationName("Scercog");

	MainWindow w;
	w.show();

	return a.exec();
}

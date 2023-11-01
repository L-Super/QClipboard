#include <QApplication>
#include <QFile>
#include "Clipboard.h"
#include "version.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/resources/images/clipboard2.svg"));
	a.setApplicationVersion(VERSION_STR);

	QFile style(":/qss/resources/style.css");
	if( style.open(QFile::ReadOnly))
	{
		qApp->setStyleSheet(style.readAll());
		style.close();
	}

	Clipboard c;
	c.show();

	return QApplication::exec();
}

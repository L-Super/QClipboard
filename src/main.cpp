#include <QApplication>
#include "Clipboard.h"
#include "version.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/resources/images/clipboard2.svg"));
	a.setApplicationVersion(VERSION_STR);

	Clipboard c;
//	c.show();

	return QApplication::exec();
}

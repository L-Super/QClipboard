#include <QApplication>
#include "Clipboard.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Clipboard c;
	c.show();

	return QApplication::exec();
}

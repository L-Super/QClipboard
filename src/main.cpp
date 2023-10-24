#include <QApplication>
#include "Clipboard.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	auto c = new Clipboard1();

	return QApplication::exec();
}

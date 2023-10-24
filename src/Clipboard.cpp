/**
 * Created by LMR on 2023/10/24.
*/


#include "Clipboard.h"

#include <QApplication>
#include <QClipboard>
#include <QDebug>



Clipboard::Clipboard(QWidget* parent)
	:
	QWidget(parent), clipboard(QApplication::clipboard())
{

}

Clipboard::~Clipboard()
{

}

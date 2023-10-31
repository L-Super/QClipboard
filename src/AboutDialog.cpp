/**
 * Created by LMR on 2023/10/31.
*/

// You may need to build the project (run Qt uic code generator) to get "ui_AboutDialog.h" resolved

#include "AboutDialog.h"
#include "ui_AboutDialog.h"


AboutDialog::AboutDialog(QWidget* parent)
	: QDialog(parent), ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
	delete ui;
}

/**
 * Created by LMR on 2023/10/31.
*/

#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget* parent)
	: QDialog(parent), ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
	ui->imagePushButton->setEnabled(false);
	ui->imagePushButton->setIcon(QIcon(":/resources/images/clipboard2.svg"));
	const QString md{"### 简介\n"
					 "一个跨平台的剪贴板工具，可以从剪贴板历史记录中选择对应的项进行粘贴。\n"
					 "### 使用方式\n"
					 "快捷键 `Alt + V`显示剪贴板\n"
					 "### 地址\n"
					 "[L-Super/QClipboard](https://github.com/L-Super/QClipboard)"
	};
	ui->textBrowser->setMarkdown(md);
	ui->textBrowser->setOpenExternalLinks(true);
}

AboutDialog::~AboutDialog()
{
	delete ui;
}

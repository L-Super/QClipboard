/**
 * Created by LMR on 2023/10/27.
*/

// You may need to build the project (run Qt uic code generator) to get "ui_Item.h" resolved

#include "Item.h"
#include "ui_Item.h"
#include <QVariant>
#include <QPixmap>
#include <QListWidgetItem>

Item::Item(QWidget* parent)
	: QWidget(parent), ui(new Ui::Item)
{
	ui->setupUi(this);

	ui->label->setWordWrap(true);
	ui->deletePushButton->setIcon(QIcon(":/resources/images/delete.svg"));
	ui->pushButton->setIcon(QIcon(":/resources/images/clipboard.svg"));

	connect(ui->deletePushButton, &QPushButton::clicked, this, &Item::DeleteButtonClicked);
}

Item::Item(const QString& text, QWidget* parent)
	: QWidget(parent), ui(new Ui::Item)
{
	ui->setupUi(this);

	ui->label->setWordWrap(true);
	ui->deletePushButton->setIcon(QIcon(":/resources/images/delete.svg"));
	ui->pushButton->setIcon(QIcon(":/resources/images/clipboard.svg"));

	ui->label->setText(text);

	connect(ui->deletePushButton, &QPushButton::clicked, this, &Item::DeleteButtonClicked);
}

Item::~Item()
{
	delete ui;
	qDebug()<<"~Item";
}
void Item::SetData(const QVariant& data)
{
	auto type = data.userType();

	if (type == QMetaType::QString) {
		qDebug() << "Item add QString" << data.toString();
		SetText(data.toString());
	}
	else if (type == QMetaType::QPixmap) {
		qDebug() << "Item add  Pixmap";

		auto pixmap = data.value<QPixmap>();
		ui->label->setPixmap(pixmap);
	}
	else if (type == QMetaType::QImage) {
		qDebug() << "Item add QImage";

		auto pixmap = QPixmap::fromImage(data.value<QImage>());
		ui->label->setPixmap(pixmap);
	}
}
void Item::SetText(const QString& text)
{
	ui->label->setText(text);
}
QString Item::GetText()
{
	return ui->label->text();
}

void Item::SetListWidgetItem(QListWidgetItem* listWidgetItem)
{
	listItem = listWidgetItem;
}
QListWidgetItem* Item::GetListWidgetItem()
{
	return listItem == nullptr ? nullptr : listItem;
}
void Item::DeleteButtonClicked()
{
	emit deleteButtonClickedSignal(GetListWidgetItem());
}

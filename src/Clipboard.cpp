/**
 * Created by LMR on 2023/10/24.
*/


#include "Clipboard.h"
#include "QHotkey"
#include "Item.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QDebug>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMimeData>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
//TODO：
// 1. 快捷键激活后，窗口置顶
// 2. 置顶后，点击非Widget区域自动隐藏
// 3. 点击item时，如果listWidget存在该item则不再添加

Clipboard::Clipboard(QWidget* parent)
	: QWidget(parent), clipboard(QApplication::clipboard()), trayIcon(new QSystemTrayIcon(this)),
	  trayMenu(new QMenu(this)), hotkey(new QHotkey()), listWidget(new QListWidget(this))
{
//	setWindowOpacity(0.8);
//	setFocus(Qt::ActiveWindowFocusReason);
	resize(360, 300);

	auto label = new QLabel("剪贴板", this);
	auto clearButton = new QPushButton("全部清除", this);
	clearButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	auto hLayout = new QHBoxLayout();
	hLayout->setSizeConstraint(QLayout::SetFixedSize);
	hLayout->addWidget(label);
	hLayout->addWidget(clearButton);

	auto layout = new QVBoxLayout(this);
	layout->addLayout(hLayout);
	layout->addWidget(listWidget);

	SetShortcut();
	InitTrayMenu();
	CreateTrayAction();


//	connect(clipboard, &QClipboard::changed, this, [this](QClipboard::Mode mode)
//	{
//		qDebug() << "changed" << mode << clipboard->text();
//	});
	connect(clipboard, &QClipboard::dataChanged, this, &Clipboard::DataChanged);
	connect(listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item)
	{
		Item* widget = qobject_cast<Item*>(listWidget->itemWidget(item));
		qDebug() << "itemClicked" << widget->GetText();
		//TODO:设置剪贴板 clip->SetText()
	});

	connect(clearButton, &QPushButton::clicked, this, &Clipboard::ClearItems);
}

Clipboard::~Clipboard()
{

}
void Clipboard::DataChanged()
{
	if (latestText == clipboard->text())
		return;
	qDebug() << "dataChanged" << clipboard->text();

	QVariant data;
	const QMimeData* mimeData = clipboard->mimeData();
	if (mimeData->hasText()) {
		latestText = mimeData->text();
		data.setValue(latestText);
		qDebug() << "latest text" << latestText;
	}
	else if (mimeData->hasImage()) {
		// 将图片数据转为QImage
		latestImage = qvariant_cast<QImage>(mimeData->imageData());
		data.setValue(latestImage);
		qDebug() << "latest image" << latestImage.width() << latestImage.height();
	}
	AddData(data);

}
void Clipboard::ClearItems()
{
	listWidget->clear();
}
void Clipboard::RemoveItem(QListWidgetItem* item)
{
	listWidget->removeItemWidget(item);
	// need to delete it, otherwise it will not disappear from the listWidget
	delete item;
}
void Clipboard::StayOnTop()
{
	qDebug("Stay on top screen");
//	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	show();
//	setFocus();
//	setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
}
void Clipboard::InitTrayMenu()
{
	//TODO:update icon
	trayIcon->setIcon(QIcon(":/resources/images/clipboard2.svg"));
	//TODO:不显示tooltip
	trayIcon->setToolTip("QClipboard");

	// 在右键时，弹出菜单。
	trayIcon->setContextMenu(trayMenu);
	trayIcon->show();
	// 在系统拖盘增加图标时显示提示信息
	trayIcon->showMessage("QClipboard 剪贴板", "已隐藏至系统托盘");
	connect(trayIcon, &QSystemTrayIcon::activated, this, &Clipboard::TrayIconActivated);
}
void Clipboard::CreateTrayAction()
{
	//TODO:完善Action
	auto aboutAction = new QAction("关于");
	auto exitAction = new QAction("退出");

	aboutAction->setIcon(QIcon(":/resources/images/info.svg"));
	exitAction->setIcon(QIcon(":/resources/images/power.svg"));

	trayMenu->addAction(aboutAction);
	trayMenu->addSeparator();
	trayMenu->addAction(exitAction);

	connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

	connect(aboutAction, &QAction::triggered, this, [this]
	{
//		// 控制着当最后一个可视的窗口退出时候，程序是否退出，默认是true
//		// 不加的话，点击后主程序也退出了
//		QApplication::setQuitOnLastWindowClosed(false);
	});
}
void Clipboard::SetShortcut()
{
	//TODO:设置不冲突的快捷键
	hotkey->setShortcut(QKeySequence("Alt+V"), true);
	connect(hotkey, &QHotkey::activated, this, &Clipboard::StayOnTop);

	// for test
	auto hideKey = new QHotkey(QKeySequence("Alt+H"), true, this);
	connect(hideKey, &QHotkey::activated, this, &Clipboard::hide);
}
void Clipboard::focusOutEvent(QFocusEvent* event)
{
	qDebug() << "lose focus and hide";
	hide();
}
void Clipboard::TrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::DoubleClick: {
		this->showNormal();
//		trayIcon->hide();
	}
		break;

	default:
		break;
	}
}
void Clipboard::AddData(const QVariant& data)
{
	auto listItem = new QListWidgetItem();
	listItem->setSizeHint(QSize(300, 80));
	auto item = new Item(this);
	item->SetData(data);
	item->SetListWidgetItem(listItem);

	connect(item, &Item::deleteButtonClickedSignal, this, &Clipboard::RemoveItem);
	connect(item, &Item::itemClickedSignal, this, &Clipboard::SetClipboardText);

	// 始终头插，且QListWidgetItem不能指定QListWidget为父对象
	listWidget->insertItem(0, listItem);
	listWidget->setItemWidget(listItem, item);

}
void Clipboard::SetClipboardText(const QString& text)
{
	qDebug() << "Clipboard::SetClipboardText" << text;
	clipboard->setText(text);
}

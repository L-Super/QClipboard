/**
 * Created by LMR on 2023/10/24.
*/


#include "Clipboard.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QDebug>


Clipboard::Clipboard(QWidget* parent)
	: QWidget(parent), clipboard(QApplication::clipboard()), trayIcon(new QSystemTrayIcon(this)),
	  trayMenu(new QMenu(this))
{

	InitTrayMenu();
	CreateTrayAction();
//	connect(clipboard, &QClipboard::changed, this, [this](QClipboard::Mode mode)
//	{
//		qDebug() << "changed" << mode << clipboard->text();
//	});
	connect(clipboard, &QClipboard::dataChanged, this, &Clipboard::DataChanged);
}

Clipboard::~Clipboard()
{

}
void Clipboard::DataChanged()
{
	qDebug() << "dataChanged" << clipboard->text();

}
void Clipboard::InitTrayMenu()
{
	//TODO:add icon
	trayIcon->setIcon(QIcon());
	trayIcon->setToolTip(("QClipboard 剪贴板"));
	// 在系统拖盘增加图标时显示提示信息
	trayIcon->showMessage("QClipboard", "已隐藏至系统托盘");
	// 在右键时，弹出菜单。
	trayIcon->setContextMenu(trayMenu);
	trayIcon->show();
}
void Clipboard::CreateTrayAction()
{
	//TODO:完善Action
	auto aboutAction = new QAction("关于");
	auto exitAction = new QAction("退出");

//	trayShowAction->setIcon(QIcon(":/image/image/home.png"));
//	exitAction->setIcon(QIcon(":/image/image/exit.png"));
//	aboutAction->setIcon(QIcon(":/image/image/about.png"));
//	cancelAction->setIcon(QIcon(":/image/image/cancel.png"));

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

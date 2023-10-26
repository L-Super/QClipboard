/**
 * Created by LMR on 2023/10/24.
*/


#include "Clipboard.h"
#include "QHotkey"
#include "spdlog/spdlog.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QDebug>

//TODO：
// 1. 快捷键激活后，窗口置顶
// 2. 置顶后，点击非Widget区域自动隐藏

Clipboard::Clipboard(QWidget* parent)
	: QWidget(parent), clipboard(QApplication::clipboard()), trayIcon(new QSystemTrayIcon(this)),
	  trayMenu(new QMenu(this)), hotkey(new QHotkey())
{
	setWindowOpacity(0.8);
	setFocus(Qt::ActiveWindowFocusReason);
	SetShortcut();
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
void Clipboard::StayOnTop()
{
	spdlog::info("Stay on top screen");
//	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	show();
//	setFocus();
//	setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
}
void Clipboard::InitTrayMenu()
{
	//TODO:update icon
	trayIcon->setIcon(QIcon(":/resources/images/clipboard2.svg"));
	trayIcon->setToolTip(("QClipboard 剪贴板"));
	// 在系统拖盘增加图标时显示提示信息
	trayIcon->showMessage("QClipboard", "已隐藏至系统托盘");
	// 在右键时，弹出菜单。
	trayIcon->setContextMenu(trayMenu);
	trayIcon->show();
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
	hotkey->setShortcut(QKeySequence("Shift+V"), true);
	connect(hotkey, &QHotkey::activated, this, &Clipboard::StayOnTop);

	// for test
	auto hideKey = new QHotkey(QKeySequence("Shift+H"), true, this);
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

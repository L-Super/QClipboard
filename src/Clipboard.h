/**
 * Created by LMR on 2023/10/24.
*/

#pragma once

#include <QWidget>

class QMenu;
class QClipboard;
class QSystemTrayIcon;

class Clipboard: public QWidget
{
Q_OBJECT

public:
	explicit Clipboard(QWidget* parent = nullptr);
	~Clipboard() override;

protected:
	void DataChanged();

private:
	void InitTrayMenu();
	void CreateTrayAction();


private:
	QClipboard *clipboard;
	QSystemTrayIcon *trayIcon;
	QMenu* trayMenu;
};
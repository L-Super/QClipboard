/**
 * Created by LMR on 2023/10/24.
*/

#pragma once

#include <QWidget>
#include <QSystemTrayIcon>

class QMenu;
class QHotkey;
class QClipboard;

class Clipboard: public QWidget
{
Q_OBJECT

public:
	explicit Clipboard(QWidget* parent = nullptr);
	~Clipboard() override;

protected slots:
	void DataChanged();
	void StayOnTop();
	void TrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
	void InitTrayMenu();
	void CreateTrayAction();
	void SetShortcut();

protected:
	void focusOutEvent(QFocusEvent * event);

private:
	QClipboard *clipboard;
	QSystemTrayIcon *trayIcon;
	QMenu* trayMenu;
	QHotkey* hotkey;
};
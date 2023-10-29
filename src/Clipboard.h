/**
 * Created by LMR on 2023/10/24.
*/

#pragma once

#include <QWidget>
#include <QSystemTrayIcon>

class QMenu;
class QHotkey;
class QClipboard;
class QListWidget;
class QListWidgetItem;

class Clipboard: public QWidget
{
Q_OBJECT

public:
	explicit Clipboard(QWidget* parent = nullptr);
	~Clipboard() override;

protected:
	void AddData(const QVariant& data);

protected slots:
	void DataChanged();
	void ClearItems();
	void RemoveItem(QListWidgetItem* item);
	void StayOnTop();
	void TrayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void SetClipboardText(const QString& text);

private:
	void InitTrayMenu();
	void CreateTrayAction();
	void SetShortcut();

protected:
	void focusOutEvent(QFocusEvent * event) override;

private:
	QClipboard *clipboard;
	QSystemTrayIcon *trayIcon;
	QMenu* trayMenu;
	QHotkey* hotkey;
	QString latestText;
	QImage latestImage;
	QPixmap latestPixmap;
	QListWidget* listWidget;
};
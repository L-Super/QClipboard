/**
 * Created by LMR on 2023/10/24.
 */

#pragma once

#include <QSet>
#include <QSystemTrayIcon>
#include <QWidget>

class QMenu;
class QHotkey;
class QClipboard;
class QListWidget;
class QListWidgetItem;

class Clipboard : public QWidget {
  Q_OBJECT

public:
  explicit Clipboard(QWidget *parent = nullptr);
  ~Clipboard() override;

protected:
  void AddData(const QVariant &data, const QByteArray &hash);

protected slots:
  void DataChanged();
  void ClearItems();
  void RemoveItem(QListWidgetItem *item);
  void StayOnTop();
  void TrayIconActivated(QSystemTrayIcon::ActivationReason reason);
  void SetClipboardText(const QString &text);
  void SetClipboardImage(const QImage &image);

private:
  void InitTrayMenu();
  void CreateTrayAction();
  void SetShortcut();

protected:
  void closeEvent(QCloseEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;

private:
  QClipboard *clipboard;
  QSystemTrayIcon *trayIcon;
  QMenu *trayMenu;
  QHotkey *hotkey;
  QString latestText;
  QByteArray latestHashValue;
  QListWidget *listWidget;
  QSet<QByteArray> hashItems;
};
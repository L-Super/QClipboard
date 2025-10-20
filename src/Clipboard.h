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
class MainWindow;

class Clipboard : public QWidget {
    Q_OBJECT

  public:
    explicit Clipboard(QWidget *parent = nullptr);
    ~Clipboard() override;

  protected:
    void AddItem(const QVariant &data, const QByteArray &hash);
    void MoveItemToTop(const QByteArray &hash);

  protected slots:
    void DataChanged();
    void ClearItems();
    void RemoveItem(QListWidgetItem *item);
    void StayOnTop();
    void TrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void OnItemClicked(QListWidgetItem *item);

  private:
    void InitTrayMenu();
    void CreateTrayAction();
    void SetShortcut();

  protected:
    void showEvent(QShowEvent *event) override;
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
    QHash<QByteArray, QListWidgetItem *> hashItemMap;
    MainWindow* homeWidget;
    QString configFilePath;
    bool ignoreNextDataChange{false};
};
//
// Created by LMR on 25-7-26.
//

#pragma once

#include <QWidget>
class QButtonGroup;
class QHotkey;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

  void SetHotkey(QHotkey *hotkey);
  void SetOnlineStatus(bool online);

protected:
  void showEvent(QShowEvent *event) override;
  void closeEvent(QCloseEvent *event) override;

private:
  Ui::MainWindow *ui;
  QButtonGroup* buttonGroup{};
  QHotkey* hotkey{};
};

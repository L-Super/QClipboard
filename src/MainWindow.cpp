//
// Created by LMR on 25-7-26.
//

#include "MainWindow.h"

#include "QHotkey"
#include "ui_MainWindow.h"

#include <QButtonGroup>
#include <QTimer>

#include "utils/AutoStartup.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow), buttonGroup(new QButtonGroup(this)) {
  ui->setupUi(this);
  buttonGroup->setExclusive(true);
  buttonGroup->addButton(ui->generalButton, 0);
  buttonGroup->addButton(ui->shortcutButton, 1);
  buttonGroup->addButton(ui->syncButton, 2);

  // default is ui->generalButton
  ui->generalButton->setIcon(QIcon(":/resources/images/home-white.svg"));
  ui->generalButton->setChecked(true);

  const QString version = qApp->applicationVersion();
  const QString md{QString("### 简介\n"
                           "一个跨平台的剪贴板工具，可以从剪贴板历史记录中选择对应的项进行粘"
                           "贴，支持不同设备之间数据实时同步。\n"
                           "### 使用方式\n"
                           "快捷键 `Alt + V`显示剪贴板\n"
                           "### 版本\n"
                           "v%1\n"
                           "### 地址\n"
                           "[L-Super/QClipboard](https://github.com/L-Super/QClipboard)")
                       .arg(version)};
  ui->textBrowser->setMarkdown(md);

  AutoStartup autoStartup;
  ui->autoStartupCheckBox->setChecked(autoStartup.IsAutoStartup());

  connect(ui->generalButton, &QToolButton::toggled, this, [this, button = ui->generalButton](bool checked) {
    if (checked) {
      button->setIcon(QIcon(":/resources/images/home-white.svg"));
    } else {
      button->setIcon(QIcon(":/resources/images/home.svg"));
    }
  });
  connect(ui->shortcutButton, &QToolButton::toggled, this, [this, button = ui->shortcutButton](bool checked) {
    if (checked) {
      button->setIcon(QIcon(":/resources/images/keyboard-white.svg"));
    } else {
      button->setIcon(QIcon(":/resources/images/keyboard.svg"));
    }
  });
  connect(ui->syncButton, &QToolButton::toggled, this, [this, button = ui->syncButton](bool checked) {
    if (checked) {
      button->setIcon(QIcon(":/resources/images/sync-white.svg"));
    } else {
      button->setIcon(QIcon(":/resources/images/sync.svg"));
    }
  });

  connect(buttonGroup, &QButtonGroup::idClicked, this, [this](auto id) { ui->stackedWidget->setCurrentIndex(id); });

  connect(ui->autoStartupCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
    AutoStartup autoStartup;
    autoStartup.SetAutoStartup(checked);
  });
  // connect(ui->keySequenceEdit, &QKeySequenceEdit::editingFinished, this,
  //         [this, hotkey]() { qDebug() << "editing finished"; });
  // connect(ui->keySequenceEdit, &QKeySequenceEdit::keySequenceChanged, this,
  //         [this](const QKeySequence &keySequence) { qDebug() << "keySequenceChanged" << keySequence; });
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::SetHotkey(QHotkey *hotkey) {
  this->hotkey = hotkey;

  ui->keySequenceEdit->setKeySequence(this->hotkey->shortcut());

  connect(ui->keySequenceConfirmButton, &QPushButton::clicked, this, [this]() {
    if (!this->hotkey)
      return;

    auto keySequence = ui->keySequenceEdit->keySequence();
    this->hotkey->setShortcut(keySequence, true);

    if (this->hotkey->isRegistered()) {
      ui->warningLabel->setText("快捷键设置成功！");
      emit shortcutChangedSignal(keySequence);
    } else {
      ui->warningLabel->setText("<span style='color:red;'>快捷键设置冲突或不符合规则，请重新设置！</span>");
    }

    ui->warningLabel->show();
    QTimer::singleShot(1000, [this]() { ui->warningLabel->clear(); });
  });
}

void MainWindow::closeEvent(QCloseEvent *event) {
  hide();
  QWidget::closeEvent(event);
}

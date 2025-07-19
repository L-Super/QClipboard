/**
 * Created by LMR on 2023/10/24.
 */

#include "Clipboard.h"
#include "AboutDialog.h"
#include "Item.h"
#include "QHotkey"

#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QCloseEvent>
#include <QCryptographicHash>
#include <QDebug>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMimeData>
#include <QPushButton>
#include <QSizePolicy>
#include <QSystemTrayIcon>
#include <QVBoxLayout>

// TODO:
//  1. 设置不冲突的快捷键, 考虑增加配置文件，支持更改快捷键
//  2. macOS适配
//  3. 点击item时，自动在光标处粘贴

Clipboard::Clipboard(QWidget *parent)
    : QWidget(parent), clipboard(QApplication::clipboard()),
      trayIcon(new QSystemTrayIcon(this)), trayMenu(new QMenu()),
      hotkey(new QHotkey()), listWidget(new QListWidget(this)) {
  setWindowOpacity(0.9);

  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint |
                 Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
  resize(360, 400);

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

  // 屏蔽水平滚动条
  listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  SetShortcut();
  CreateTrayAction();
  InitTrayMenu();

  qApp->installEventFilter(this);

  connect(clipboard, &QClipboard::dataChanged, this, &Clipboard::DataChanged);
  connect(listWidget, &QListWidget::itemClicked, this,
          [this](QListWidgetItem *item) {
            Item *widget = qobject_cast<Item *>(listWidget->itemWidget(item));

            if (auto text = widget->GetText(); !text.isEmpty()) {
              SetClipboardText(text);
              this->hide();
              return;
            }

            if (auto image = widget->GetImage(); !image.isNull()) {
              SetClipboardImage(image);
              this->hide();
              return;
            }
          });

  connect(clearButton, &QPushButton::clicked, this, &Clipboard::ClearItems);
}

Clipboard::~Clipboard() {}

void Clipboard::DataChanged() {
  if (clipboard->text().isEmpty()) {
    return;
  }

  QVariant data;
  QByteArray hashValue;
  const QMimeData *mimeData = clipboard->mimeData();

  if (mimeData->hasText()) {
    latestText = mimeData->text();
    data.setValue(latestText);
    hashValue =
        QCryptographicHash::hash(latestText.toUtf8(), QCryptographicHash::Md5);
  } else if (mimeData->hasImage()) {
    // 将图片数据转为QImage
    auto image = qvariant_cast<QImage>(mimeData->imageData());
    QByteArray ba;
    QBuffer buffer(&ba);
    image.save(&buffer, "PNG");
    hashValue = QCryptographicHash::hash(ba, QCryptographicHash::Md5);

    data.setValue(image);
  }

  // 如果已存在，则把对应 item 搬到最前面
  if (hashItemMap.contains(hashValue)) {
    MoveDataToFront(hashValue);
    return;
  }

  AddData(data, hashValue);
}

void Clipboard::ClearItems() {
  listWidget->clear();
  clipboard->clear();
  hashItems.clear();
}

void Clipboard::RemoveItem(QListWidgetItem *item) {
  Item *widget = qobject_cast<Item *>(listWidget->itemWidget(item));
  auto value = widget->GetHashValue();
  hashItems.remove(value);
  hashItemMap.remove(value);

  listWidget->removeItemWidget(item);
  // need to delete it, otherwise it will not disappear from the listWidget
  delete item;
}

void Clipboard::StayOnTop() {
  activateWindow();
  show();
}

void Clipboard::InitTrayMenu() {
  // TODO:update icon
  trayIcon->setIcon(QIcon(":/resources/images/clipboard2.svg"));
  // 在右键时，弹出菜单。
  trayIcon->setContextMenu(trayMenu);
  trayIcon->setToolTip("QClipboard");
  trayIcon->show();
  // 在系统拖盘增加图标时显示提示信息
  trayIcon->showMessage("QClipboard 剪贴板", "已隐藏至系统托盘");

  connect(trayIcon, &QSystemTrayIcon::activated, this,
          &Clipboard::TrayIconActivated);
}

void Clipboard::CreateTrayAction() {
  auto aboutAction = new QAction("关于");
  auto exitAction = new QAction("退出");

  aboutAction->setIcon(QIcon(":/resources/images/info.svg"));
  exitAction->setIcon(QIcon(":/resources/images/power.svg"));

  trayMenu->addAction(aboutAction);
  trayMenu->addSeparator();
  trayMenu->addAction(exitAction);

  connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

  connect(aboutAction, &QAction::triggered, this, [] {
    AboutDialog aboutDialog;
    aboutDialog.exec();
  });
}

void Clipboard::SetShortcut() {
  auto isSuccess = hotkey->setShortcut(QKeySequence("Alt+V"), true);
  // TODO: add tips and change shortcut
  if (!isSuccess) {
    qCritical() << "set shortcut failed";
  }
  connect(hotkey, &QHotkey::activated, this, &Clipboard::StayOnTop);
}

void Clipboard::TrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
  switch (reason) {
  case QSystemTrayIcon::Trigger:
  case QSystemTrayIcon::DoubleClick: {
    this->showNormal();
  } break;

  default:
    break;
  }
}

void Clipboard::AddData(const QVariant &data, const QByteArray &hash) {
  auto listItem = new QListWidgetItem();
  listItem->setSizeHint(QSize(300, 80));
  auto item = new Item(this);
  item->SetData(data, hash);
  item->SetListWidgetItem(listItem);

  connect(item, &Item::deleteButtonClickedSignal, this, &Clipboard::RemoveItem);

  // 始终头插，且QListWidgetItem不能指定QListWidget为父对象
  listWidget->insertItem(0, listItem);
  listWidget->setItemWidget(listItem, item);

  // 记录 hash 与 listItem 的映射
  hashItems.insert(hash);
  hashItemMap.insert(hash, listItem);
}

void Clipboard::MoveDataToFront(const QByteArray &hashValue) {
  auto listItem = hashItemMap.value(hashValue);
  auto widget = listWidget->itemWidget(listItem);
  int row = listWidget->row(listItem);

  listWidget->takeItem(row);
  listWidget->insertItem(0, listItem);
  listWidget->setItemWidget(listItem, widget);
}

void Clipboard::SetClipboardText(const QString &text) const {
  clipboard->setText(text);
}

void Clipboard::SetClipboardImage(const QImage &image) const {
  clipboard->setImage(image);
}

void Clipboard::closeEvent(QCloseEvent *event) {
  hide();
  event->ignore();
}

bool Clipboard::eventFilter(QObject *obj, QEvent *event) {
  // 窗口停用
  if (QEvent::WindowDeactivate == event->type()) {
    hide();
    return true;
  }

  return QWidget::eventFilter(obj, event);
}

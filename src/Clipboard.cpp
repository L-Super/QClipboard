/**
 * Created by LMR on 2023/10/24.
 */

#include "Clipboard.h"
#include "AboutDialog.h"
#include "Item.h"
#include "MainWindow.h"
#include "QHotkey"
#include "utils/Config.h"

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
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QSizePolicy>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QVBoxLayout>

// TODO:
//  1. 设置不冲突的快捷键, 考虑增加配置文件，支持更改快捷键
//  2. macOS适配
//  3. 点击item时，自动在光标处粘贴

Clipboard::Clipboard(QWidget *parent)
    : QWidget(parent), clipboard(QApplication::clipboard()), trayIcon(new QSystemTrayIcon(this)), trayMenu(new QMenu()),
      hotkey(new QHotkey(this)), listWidget(new QListWidget(this)), homeWidget(new MainWindow()) {
  setWindowOpacity(0.9);

  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint |
                 Qt::WindowStaysOnTopHint);
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

  configFilePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/clipboard_settings.json";

  SetShortcut();
  homeWidget->SetHotkey(hotkey);
  CreateTrayAction();
  InitTrayMenu();

  qApp->installEventFilter(this);

  connect(clipboard, &QClipboard::dataChanged, this, &Clipboard::DataChanged);
  connect(listWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *listWidgetItem) {
    Item *item = qobject_cast<Item *>(listWidget->itemWidget(listWidgetItem));

    switch (item->GetMetaType()) {
    case QMetaType::QString: {
      clipboard->setText(item->GetText());
      this->hide();
    } break;
    case QMetaType::QImage: {
      clipboard->setImage(item->GetImage());
      this->hide();
    } break;
    default:
      break;
    }
  });

  connect(clearButton, &QPushButton::clicked, this, &Clipboard::ClearItems);
  connect(homeWidget, &MainWindow::shortcutChangedSignal, this, [this](const QKeySequence &keySequence) {
    Config config(configFilePath.toStdString());
    auto value = keySequence.toString().toStdString();
    config.set("shortcut", value);
  });
}

Clipboard::~Clipboard() { homeWidget->deleteLater(); }

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
    hashValue = QCryptographicHash::hash(latestText.toUtf8(), QCryptographicHash::Md5);
  } else if (mimeData->hasImage()) {
    // 将图片数据转为QImage
    auto image = qvariant_cast<QImage>(mimeData->imageData());
    QByteArray ba;
    QBuffer buffer(&ba);
    image.save(&buffer, "PNG");
    hashValue = QCryptographicHash::hash(ba, QCryptographicHash::Md5);

    data.setValue(image);
  } else if (mimeData->hasUrls()) {
    qDebug() << "has urls" << mimeData->urls();
  }

  // 如果已存在，则把对应 item 搬到最前面
  if (hashItemMap.contains(hashValue)) {
    MoveItemToTop(hashValue);
    return;
  }

  AddItem(data, hashValue);
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
  // TODO: 窗口出现在输入光标位置
  show();
  activateWindow();
  raise();
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

  connect(trayIcon, &QSystemTrayIcon::activated, this, &Clipboard::TrayIconActivated);
}

void Clipboard::CreateTrayAction() {
  auto homeAction = new QAction("主界面");
  auto aboutAction = new QAction("关于");
  auto exitAction = new QAction("退出");

  homeAction->setIcon(QIcon(":/resources/images/action-home.svg"));
  aboutAction->setIcon(QIcon(":/resources/images/info.svg"));
  exitAction->setIcon(QIcon(":/resources/images/power.svg"));

  trayMenu->addAction(homeAction);
  trayMenu->addAction(aboutAction);
  trayMenu->addSeparator();
  trayMenu->addAction(exitAction);

  connect(homeAction, &QAction::triggered, this, [this] {
    homeWidget->show();
    homeWidget->raise();
  });

  connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

  connect(aboutAction, &QAction::triggered, this, [] {
    AboutDialog aboutDialog;
    aboutDialog.exec();
  });
}

void Clipboard::SetShortcut() {
  Config config(configFilePath.toStdString());
  // 从配置文件读取快捷键
  QString shortcutStr = "Alt+V"; // 默认快捷键
  // 将快捷键在配置文件中存为一个字符串，例如 "Alt+V" 或 "Ctrl+Shift+V"
  if (auto op = config.get<std::string>("shortcut"); op.has_value() && !op->empty()) {
    shortcutStr = QString::fromStdString(*op);
  }

  // 设置快捷键
  hotkey->setShortcut(QKeySequence(shortcutStr), true);
  if (!hotkey->isRegistered()) {
    QMessageBox::warning(this, "快捷键设置失败",
                         QString("无法设置快捷键: %1\n请检查是否与其他程序冲突。").arg(shortcutStr));
    return;
  }
  qDebug() << "Registered global shortcut is" << shortcutStr << hotkey->shortcut();
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

void Clipboard::AddItem(const QVariant &data, const QByteArray &hash) {
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

void Clipboard::MoveItemToTop(const QByteArray &hashValue) {
  auto listItem = hashItemMap.value(hashValue);
  if (!listItem)
    return;

  int row = listWidget->row(listItem);
  if (row == 0)
    return;

  // 把 row 移动到 parent() 下的 0 位置
  listWidget->model()->moveRow(QModelIndex(), row, QModelIndex(), 0);

  // TODO:或许指针地址改变
  //   hashItemMap.remove(hashValue);
  //   hashItemMap.insert(hashValue, listItem);
}

void Clipboard::closeEvent(QCloseEvent *event) {
  hide();
  event->ignore();
}

bool Clipboard::eventFilter(QObject *obj, QEvent *event) {

  if (event->type() == QEvent::WindowDeactivate) {
    // 窗口停用
    hide();
    return true;
  } else if (event->type() == QEvent::KeyPress) {
    // 按ESC键时隐藏窗口
    auto *keyEvent = dynamic_cast<QKeyEvent *>(event);
    if (keyEvent->key() == Qt::Key_Escape) {
      hide();
      return true;
    }
  }
  return QWidget::eventFilter(obj, event);
}

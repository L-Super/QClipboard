/**
 * Created by LMR on 2023/10/24.
 */

#include "Clipboard.h"
#include "AboutDialog.h"
#include "Item.h"
#include "MainWindow.h"
#include "QHotkey"

#ifdef ENABLE_SYNC
#include "net/SyncServer.h"
#endif

#include "utils/Config.h"
#include "utils/Logger.hpp"
#include "utils/Util.h"

#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QCloseEvent>
#include <QCryptographicHash>
#include <QDebug>
#include <QDesktopServices>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QSizePolicy>
#include <QSystemTrayIcon>
#include <QVBoxLayout>

Clipboard::Clipboard(QWidget* parent)
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

  InitShortcut();
  homeWidget->SetHotkey(hotkey);
  CreateTrayAction();
  InitTrayMenu();

#ifdef ENABLE_SYNC
  InitSyncServer();
#endif

  qApp->installEventFilter(this);

  connect(clipboard, &QClipboard::dataChanged, this, &Clipboard::DataChanged);
  connect(listWidget, &QListWidget::itemClicked, this, &Clipboard::OnItemClicked);
  connect(clearButton, &QPushButton::clicked, this, &Clipboard::ClearItems);
}

Clipboard::~Clipboard() { homeWidget->deleteLater(); }

#ifdef ENABLE_SYNC
void Clipboard::ReloadSyncServer() {
  if (InitSyncServer()) {
    spdlog::info("ReloadSyncServer success");
  }
  else {
    spdlog::error("ReloadSyncServer failed");
    if (auto url = Config::instance().get<std::string>("url"); url.has_value()) {
      QDesktopServices::openUrl(QUrl(QString::fromStdString(url.value())));
    }
  }
}
#endif

void Clipboard::DataChanged() {
  if (ignoreNextDataChange) {
    ignoreNextDataChange = false;
    // 忽略这次信号
    return;
  }

  QByteArray hashValue;
  ClipboardData clipData;
  ClipboardSourceInfo sourceInfo;

  const QMimeData* mimeData = clipboard->mimeData();
  qDebug() << "mime data type:" << mimeData->formats();

  sourceInfo.timestamp = QDateTime::currentDateTime();
  sourceInfo.processPath = utils::GetClipboardSourceAppPath();
  sourceInfo.processName = utils::GetAppName(sourceInfo.processPath);
  sourceInfo.icon = utils::GetAppIcon(sourceInfo.processPath);

  if (mimeData->hasText()) {
    const QString latestText = mimeData->text();

    sourceInfo.data = latestText;
    hashValue = QCryptographicHash::hash(latestText.toUtf8(), QCryptographicHash::Md5);

    clipData.type = ClipboardDataType::text;
    clipData.data = latestText.toUtf8();
  }
  else if (mimeData->hasImage()) {
    // 将图片数据转为QImage
    auto image = qvariant_cast<QImage>(mimeData->imageData());
    qDebug() << "image format" << image.format();
    QByteArray ba;
    QBuffer buffer(&ba);
    image.save(&buffer, "PNG");
    hashValue = QCryptographicHash::hash(ba, QCryptographicHash::Md5);

    sourceInfo.data = image;

    clipData.type = ClipboardDataType::image;
    clipData.data = ba;
  }
  else if (mimeData->hasUrls()) {
    qDebug() << "has urls" << mimeData->urls();
  }

  if (sourceInfo.data.isNull() || hashValue.isEmpty())
    return;

  // If it already exists, move the corresponding item to the front.
  if (hashItemMap.contains(hashValue)) {
    MoveItemToTop(hashValue);
    return;
  }

  AddItem(sourceInfo, hashValue);

  if (ignoreNetDataChange) {
    ignoreNetDataChange = false;
    return;
  }

#ifdef ENABLE_SYNC
  if (sync)
    sync->uploadClipboardData(clipData);
#endif
}

void Clipboard::ClearItems() {
  listWidget->clear();
  clipboard->clear();
  hashItems.clear();
}

void Clipboard::RemoveItem(QListWidgetItem* item) {
  Item* widget = qobject_cast<Item*>(listWidget->itemWidget(item));
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
  trayIcon->setIcon(QIcon(":/resources/icon.png"));
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
#ifdef ENABLE_SYNC
    if (sync) {
      homeWidget->SetOnlineStatus(sync->isLoggedIn());
    }
    else {
      homeWidget->SetOnlineStatus(false);
    }
#endif

    homeWidget->show();
    homeWidget->raise();
  });

  connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

  connect(aboutAction, &QAction::triggered, this, [] {
    AboutDialog aboutDialog;
    aboutDialog.exec();
  });
}

void Clipboard::InitShortcut() {
  Config& config = Config::instance();
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
  spdlog::info("Registered global shortcut is {}", shortcutStr);
  connect(hotkey, &QHotkey::activated, this, &Clipboard::StayOnTop);
}

#ifdef ENABLE_SYNC
bool Clipboard::InitSyncServer() {
  auto userInfo = Config::instance().getUserInfo();
  if (!userInfo.has_value()) {
    spdlog::warn("User info not exist");
    return false;
  }

  if (auto url = Config::instance().get<std::string>("url"); url.has_value()) {
    if (sync) {
      sync->stopSync();
      sync.reset();
    }
    sync = std::make_unique<SyncServer>(QString::fromStdString(url.value()));
    if (!sync->setToken(QString::fromStdString(userInfo.value().token))) {
      spdlog::warn("Token is invalid");
      sync.reset();
      return false;
    }

    connect(sync.get(), &SyncServer::registrationFinished, [] {});
    connect(sync.get(), &SyncServer::loginFinished, [this](bool success, const Token& token, const QString& message) {
      if (success)
        qDebug() << "login successful";
      else
        qDebug() << "login failed." << message;
    });
    connect(sync.get(), &SyncServer::uploadFinished, [](bool success, const QString& message) {
      if (success) {
        const auto doc = QJsonDocument::fromJson(message.toUtf8());
        const auto obj = doc.object();
        int id = obj.value("id").toInt();
        QString created = obj.value("created_at").toString();

        spdlog::info("Upload data successful, id: {} created_at: {}", id, created);
      }
      else {
        spdlog::error("Upload data failed. {}", message);
      }
    });
    connect(sync.get(), &SyncServer::imageDownloadFinished,
            [this](bool success, const QImage& image, const QString& message) {
              if (success) {
                spdlog::info("Image downloaded successfully, updating clipboard");
                // 忽略下一次dataChanged信号
                ignoreNetDataChange = true;
                clipboard->setImage(image);
              }
              else {
                spdlog::warn("Image download failed: {}", message);
              }
            });
    connect(sync.get(), &SyncServer::notifyMessageReceived, [this](const QString& message) {
      // json:
      //{
      //  "action":"update",
      //  "type":"", // text, image, file
      //  "data":"",
      //  "data_hash":"",
      //  "meta":{}
      //}

      spdlog::debug("Websocket received: {}", message);
      spdlog::info("Message received from websocket");
      try {
        const auto doc = QJsonDocument::fromJson(message.toUtf8());
        const auto obj = doc.object();
        auto type = obj.value("type").toString();
        auto data = obj.value("data").toString();

        if (type == "text") {
          if (!data.isEmpty()) {
            // 忽略下一次dataChanged信号
            ignoreNetDataChange = true;
            clipboard->setText(data);
          }
        }
        else if (type == "image") {
          // 当接收到image类型时，data字段为图片的URL
          const auto& imageUrl = data;
          spdlog::info("Received image url:{}", imageUrl);

          // 调用下载逻辑
          if (sync) {
            sync->downloadImage(imageUrl);
          }
        }
        else if (type == "file") {
        }
      }
      catch (const std::exception& e) {
        spdlog::error("Websocket received data has exception. {}", e.what());
      }
    });
    connect(sync.get(), &SyncServer::syncConnected, [] {});
    connect(sync.get(), &SyncServer::syncDisconnected, [] {});
    connect(sync.get(), &SyncServer::syncError, [] {});
    return true;
  }
  return false;
}
#endif

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

void Clipboard::OnItemClicked(QListWidgetItem* listWidgetItem) {
  Item* item = qobject_cast<Item*>(listWidget->itemWidget(listWidgetItem));

  // 设置标志位，忽略下一次dataChanged信号
  ignoreNextDataChange = true;

  switch (item->GetMetaType()) {
  case QMetaType::QString: {
    clipboard->setText(item->GetText());
  } break;
  case QMetaType::QImage: {
    clipboard->setImage(item->GetImage());
  } break;
  default:
    break;
  }

  // 先移动item到顶部，这会自动设置当前行和滚动位置
  MoveItemToTop(item->GetHashValue());

  hide();
}

void Clipboard::AddItem(const ClipboardSourceInfo& data, const QByteArray& hash) {
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

void Clipboard::MoveItemToTop(const QByteArray& hashValue) {
  auto listItem = hashItemMap.value(hashValue);
  if (!listItem)
    return;

  int row = listWidget->row(listItem);
  if (row == 0)
    return;

  // 把 row 移动到 parent() 下的 0 位置
  listWidget->model()->moveRow(QModelIndex(), row, QModelIndex(), 0);

  // 移动后，确保选中第一项（索引0）
  listWidget->setCurrentRow(0);
  listWidget->scrollToTop();
}

void Clipboard::showEvent(QShowEvent* event) {
  listWidget->setFocusPolicy(Qt::StrongFocus);
  listWidget->setFocus();

  // 设置默认选中第一项并滚动到顶部
  if (listWidget->count() > 0) {
    if (listWidget->currentRow() != 0) {
      listWidget->setCurrentRow(0);
    }

    listWidget->scrollToTop();
  }

  QWidget::showEvent(event);
}

void Clipboard::closeEvent(QCloseEvent* event) {
  hide();
  event->ignore();
}

bool Clipboard::eventFilter(QObject* obj, QEvent* event) {

  if (event->type() == QEvent::WindowDeactivate) {
    // 窗口停用
    hide();
    return true;
  }
  else if (event->type() == QEvent::KeyPress) {
    // 按ESC键时隐藏窗口
    auto* keyEvent = dynamic_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Escape) {
      hide();
      return true;
    }
    if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
      OnItemClicked(listWidget->currentItem());
      return true;
    }
  }
  return QWidget::eventFilter(obj, event);
}

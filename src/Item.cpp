/**
 * Created by LMR on 2023/10/27.
 */

#include "Item.h"
#include "ui_Item.h"
#include <QDebug>
#include <QListWidgetItem>
#include <QPixmap>
#include <QStyleHints>
#include <QVariant>

Item::Item(QWidget* parent) : QWidget(parent), ui(new Ui::Item) {
  ui->setupUi(this);

  ui->label->setWordWrap(true);
  ui->label->setAlignment(Qt::AlignTop);
  ui->deletePushButton->setIcon(QIcon(":/resources/images/delete.svg"));
  //	ui->pushButton->setIcon(QIcon(":/resources/images/clipboard.svg"));

  ui->pushButton->hide();

  ApplyTheme(QGuiApplication::styleHints()->colorScheme());

  // 连接系统主题变化信号 Qt 6.5 support
  connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, this, &Item::ApplyTheme);
  connect(ui->deletePushButton, &QPushButton::clicked, this, &Item::DeleteButtonClicked);
}

Item::Item(const QString& text, QWidget* parent) : Item(parent) {
  //
  ui->label->setText(text);
}

Item::~Item() { delete ui; }

void Item::SetData(const ClipboardSourceInfo& source_info, const QByteArray& hash) {
  metaType = source_info.data.userType();

  if (metaType == QMetaType::QString) {
    SetText(source_info.data.toString());
  }
  else if (metaType == QMetaType::QPixmap) {
    qDebug() << "Item add  Pixmap";

    auto pixmap = source_info.data.value<QPixmap>();
    ui->label->setPixmap(pixmap);
  }
  else if (metaType == QMetaType::QImage) {
    qDebug() << "Item add QImage";

    latestImage = source_info.data.value<QImage>();

    auto pixmap = latestImage.scaled(this->size(), Qt::KeepAspectRatio, Qt::FastTransformation);
    ui->label->setPixmap(QPixmap::fromImage(pixmap));
  }

  hashValue = hash;
}

void Item::SetText(const QString& text) { ui->label->setText(text); }

QString Item::GetText() const { return ui->label->text(); }

void Item::SetListWidgetItem(QListWidgetItem* listWidgetItem) { listItem = listWidgetItem; }

QListWidgetItem* Item::GetListWidgetItem() const { return listItem == nullptr ? nullptr : listItem; }

void Item::DeleteButtonClicked() { emit deleteButtonClickedSignal(GetListWidgetItem()); }

void Item::ApplyTheme(Qt::ColorScheme scheme) {
  switch (scheme) {
  case Qt::ColorScheme::Dark:
    ui->deletePushButton->setIcon(QIcon(":/resources/images/delete-white.svg"));
    break;
  case Qt::ColorScheme::Light:
    ui->deletePushButton->setIcon(QIcon(":/resources/images/delete.svg"));
    break;
  case Qt::ColorScheme::Unknown:
    ui->deletePushButton->setIcon(QIcon(":/resources/images/delete.svg"));
    break;
  }
}

QImage Item::GetImage() const { return latestImage; }

QByteArray Item::GetHashValue() const { return hashValue; }

int Item::GetMetaType() const { return metaType; }

/**
 * Created by LMR on 2023/10/27.
 */

#include "Item.h"
#include "ui_Item.h"
#include <QDebug>
#include <QListWidgetItem>
#include <QPixmap>
#include <QVariant>

Item::Item(QWidget *parent) : QWidget(parent), ui(new Ui::Item) {
  ui->setupUi(this);

  ui->label->setWordWrap(true);
  ui->label->setAlignment(Qt::AlignTop);
  ui->deletePushButton->setIcon(QIcon(":/resources/images/delete.svg"));
  //	ui->pushButton->setIcon(QIcon(":/resources/images/clipboard.svg"));

  connect(ui->deletePushButton, &QPushButton::clicked, this,
          &Item::DeleteButtonClicked);
}

Item::Item(const QString &text, QWidget *parent)
    : QWidget(parent), ui(new Ui::Item) {
  ui->setupUi(this);

  ui->label->setWordWrap(true);
  ui->label->setAlignment(Qt::AlignTop);
  ui->deletePushButton->setIcon(QIcon(":/resources/images/delete.svg"));
  //	ui->pushButton->setIcon(QIcon(":/resources/images/clipboard.svg"));

  ui->label->setText(text);

  connect(ui->deletePushButton, &QPushButton::clicked, this,
          &Item::DeleteButtonClicked);
}

Item::~Item() { delete ui; }

void Item::SetData(const QVariant &data, const QByteArray &hash) {
  metaType = data.userType();

  if (metaType == QMetaType::QString) {
    SetText(data.toString());
  } else if (metaType == QMetaType::QPixmap) {
    qDebug() << "Item add  Pixmap";

    auto pixmap = data.value<QPixmap>();
    ui->label->setPixmap(pixmap);
  } else if (metaType == QMetaType::QImage) {
    latestImage = data.value<QImage>();
    ui->label->setPixmap(QPixmap::fromImage(latestImage));
  }

  hashValue = hash;
}

void Item::SetText(const QString &text) { ui->label->setText(text); }

QString Item::GetText() const { return ui->label->text(); }

void Item::SetListWidgetItem(QListWidgetItem *listWidgetItem) {
  listItem = listWidgetItem;
}

QListWidgetItem *Item::GetListWidgetItem() const {
  return listItem == nullptr ? nullptr : listItem;
}

void Item::DeleteButtonClicked() {
  emit deleteButtonClickedSignal(GetListWidgetItem());
}

QImage Item::GetImage() const { return latestImage; }

QByteArray Item::GetHashValue() const { return hashValue; }

int Item::GetMetaType() const { return metaType; }

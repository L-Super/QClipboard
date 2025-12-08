//
// Created by LMR on 2025/12/7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_CustomToolTip.h" resolved

#include "CustomToolTip.h"
#include "ui_CustomToolTip.h"

#include <QPainter>
#include <QPainterPath>
#include <QRegion>
#include <QStyleOption>

CustomToolTip::CustomToolTip(QWidget* parent) : QWidget(parent), ui(new Ui::CustomToolTip) {
  ui->setupUi(this);
  setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setAutoFillBackground(false);
}

CustomToolTip::~CustomToolTip() { delete ui; }

void CustomToolTip::SetData(const ClipboardSourceInfo& sourceInfo) {
  ui->iconLabel->setPixmap(sourceInfo.icon.pixmap(sourceInfo.icon.actualSize(QSize(16, 16))));
  ui->appLabel->setText(sourceInfo.processName);
  ui->timestampLabel->setText(sourceInfo.timestamp.toString("MM-dd hh:mm:ss"));
  ui->typeLabel->setText(sourceInfo.data.metaType().name());
}

void CustomToolTip::paintEvent(QPaintEvent* event) {
  QWidget::paintEvent(event);

  QStyleOption opt;
  opt.initFrom(this);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  // 圆角背景
  const qreal radius = 10.0;
  QRectF bgRect = QRectF(this->rect()).adjusted(0.5, 0.5, -0.5, -0.5);
  QPainterPath bgPath;
  bgPath.addRoundedRect(bgRect, radius, radius);

  // 透明背景，避免黑底
  painter.fillRect(rect(), Qt::transparent);

  painter.setPen(Qt::NoPen);
  painter.setBrush(opt.palette.window());
  painter.drawPath(bgPath);

  // 轻描边，增强边缘清晰度
  painter.setPen(QPen(opt.palette.mid(), 1));
  painter.setBrush(Qt::NoBrush);
  painter.drawPath(bgPath);

  // 应用圆角遮罩，去掉矩形四角
  setMask(QRegion(bgPath.toFillPolygon().toPolygon()));
}
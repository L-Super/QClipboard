/**
 * Created by LMR on 2023/10/31.
 */

#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent), ui(new Ui::AboutDialog) {
  ui->setupUi(this);
  ui->imageLabel->setPixmap(QPixmap(":/resources/icon.png").scaled({80, 80}, Qt::KeepAspectRatio, Qt::SmoothTransformation));

  const QString version = qApp->applicationVersion();

  const QString md{QString("### 简介\n"
                           "一个跨平台的剪贴板工具，可以从剪贴板历史记录中选择对应的项进行粘"
                           "贴，支持不同设备之间数据实时同步。\n"
                           "### 使用方式\n"
                           "快捷键 `Alt + V`显示剪贴板\n"
                           "### 版本\n"
                           "v%1\n"
                           "### 地址\n"
                           "[L-Super/Floward](https://github.com/L-Super/Floward)")
                       .arg(version)};
  ui->textBrowser->setMarkdown(md);

  ui->textBrowser->setOpenExternalLinks(true);
}

AboutDialog::~AboutDialog() { delete ui; }

/**
 * Created by LMR on 2023/10/31.
 */

#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutDialog) {
  ui->setupUi(this);
  ui->imagePushButton->setEnabled(false);
  ui->imagePushButton->setIcon(QIcon(":/resources/icon.png"));
  const QString version = qApp->applicationVersion();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  const QString md{
      QString("### 简介\n"
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
#else
  QString html{R"(<html><head><meta charset="utf-8"><title>about</title></head>
<body>
    <h3>简介</h3>
    <p>一个跨平台的剪贴板工具，可以从剪贴板历史记录中选择对应的项进行粘贴。</p>
    <h3>使用方式</h3>
    <p>快捷键 Alt + V 显示剪贴板</p>
    <h3>版本</h3>
    <p>v)"};
  html.append(version);
  html.append(R"(</p> <h3>地址</h3>
    <p><a href="https://github.com/L-Super/QClipboard">QClipboard</a></p>
</body></html>)");
  ui->textBrowser->setHtml(html);
#endif
  ui->textBrowser->setOpenExternalLinks(true);
}

AboutDialog::~AboutDialog() { delete ui; }

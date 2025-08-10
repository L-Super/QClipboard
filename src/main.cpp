#include "Clipboard.h"
#include "SingleApplication"
#include "utils/Config.h"
#include "version.h"

#include <QApplication>
#include <QFile>
#include <QStandardPaths>

int main(int argc, char *argv[]) {
  SingleApplication a(argc, argv);
  a.setWindowIcon(QIcon(":/resources/icon.png"));
  a.setApplicationVersion(VERSION_STR);

  QFile style(":/qss/resources/style.css");
  if (style.open(QFile::ReadOnly)) {
    qApp->setStyleSheet(style.readAll());
    style.close();
  }

  // 控制着当最后一个可视的窗口退出时候，程序是否退出，默认是true
  QApplication::setQuitOnLastWindowClosed(false);

  auto configFilePath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/clipboard_settings.json";
  Config::instance().load(configFilePath.toStdString());

  Clipboard c;
  c.show();

  QObject::connect(&a, &SingleApplication::instanceStarted, &c,
                   &Clipboard::show);

  return QApplication::exec();
}

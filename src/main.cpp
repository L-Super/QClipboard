#include "Clipboard.h"
#include "SingleApplication"
#include "version.h"
#include <QApplication>
#include <QFile>
#include <QStyleHints>

void LoadStyleSheet(const QString &stylePath) {
  QFile style(stylePath);
  if (style.open(QFile::ReadOnly)) {
    qApp->setStyleSheet(style.readAll());
  }
  style.close();
}

void ApplyTheme(Qt::ColorScheme scheme) {
  switch (scheme) {
  case Qt::ColorScheme::Dark:
    LoadStyleSheet(":/qss/resources/style_dark.css");
    break;
  case Qt::ColorScheme::Light:
    LoadStyleSheet(":/qss/resources/style.css");
  case Qt::ColorScheme::Unknown:
    LoadStyleSheet(":/qss/resources/style.css");
    break;
  }
  qDebug() << "ApplyTheme to" << scheme;
}

int main(int argc, char *argv[]) {
  SingleApplication a(argc, argv);
#ifdef Q_OS_MACOS
  a.setWindowIcon(QIcon(":/resources/icon-for-mac.png"));
#else
  a.setWindowIcon(QIcon(":/resources/icon.png"));
#endif

  a.setApplicationVersion(VERSION_STR);

  ApplyTheme(QGuiApplication::styleHints()->colorScheme());

  // 控制着当最后一个可视的窗口退出时候，程序是否退出，默认是true
  QApplication::setQuitOnLastWindowClosed(false);

  Clipboard c;
  c.show();

  QObject::connect(&a, &SingleApplication::instanceStarted, &c, &Clipboard::show);
  // 连接系统主题变化信号 Qt 6.5 support
  QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, [](Qt::ColorScheme scheme) {
    qDebug() << "System theme change to" << scheme;
    ApplyTheme(scheme);
  });

  return QApplication::exec();
}

// -----------------------------------------------------------------
// securechat_qt - an encrypted chat using OpenSSL, with a QT interface
// Copyright (C) 2019  Gabriele Bonacini
//
// This program is free software for no profit use; you can redistribute
// it and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
// A commercial license is also available for a lucrative use.
// -----------------------------------------------------------------

#include "mainwindow.h"
#include "dialogconf.h"
#include "dialoghelp.h"

#include <QApplication>
#include <QStyleFactory>

#include <QTextBlock>
#include <QTextCursor>


int main(int argc, char *argv[]){
   QApplication a(argc, argv);

   qApp->setStyle(QStyleFactory::create("Fusion"));

   QPalette darkPalette;

   darkPalette.setColor(QPalette::Window, QColor(56,56,56));
   darkPalette.setColor(QPalette::WindowText, Qt::green);
   darkPalette.setColor(QPalette::Base, QColor(28,28,28));
   darkPalette.setColor(QPalette::AlternateBase, QColor(56,56,56));
   darkPalette.setColor(QPalette::ToolTipBase, Qt::green);
   darkPalette.setColor(QPalette::ToolTipText, Qt::green);
   darkPalette.setColor(QPalette::Text, Qt::green);
   darkPalette.setColor(QPalette::Button, QColor(52,52,52));
   darkPalette.setColor(QPalette::ButtonText, Qt::green);
   darkPalette.setColor(QPalette::BrightText, Qt::red);
   darkPalette.setColor(QPalette::Link, QColor(47, 20, 220));
   darkPalette.setColor(QPalette::Highlight, QColor(47, 20, 220));
   darkPalette.setColor(QPalette::HighlightedText, Qt::black);

   qApp->setPalette(darkPalette);
   qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
   qApp->setStyleSheet("QMenuBar { color: #00ff00; background-color: #080808; border: 1px solid green; }");

   qRegisterMetaType<QTextBlock>("QTextBlock");
   qRegisterMetaType<QTextCursor>("QTextCursor");

   MainWindow w;
   w.show();

   return a.exec();
}

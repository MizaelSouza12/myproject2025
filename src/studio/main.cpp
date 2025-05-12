#include <QApplication>
#include <QStyleFactory>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Configura o estilo dark
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    app.setPalette(darkPalette);

    // Configura o estilo das abas
    app.setStyleSheet(R"(
        QTabWidget::pane {
            border: 1px solid #444;
            top: -1px;
            background: rgb(53, 53, 53);
        }

        QTabBar::tab {
            background: rgb(45, 45, 45);
            border: 1px solid #444;
            padding: 5px;
            min-width: 100px;
        }

        QTabBar::tab:selected {
            background: rgb(53, 53, 53);
            margin-bottom: -1px;
        }

        QDockWidget {
            border: 1px solid #444;
            titlebar-close-icon: url(close.png);
            titlebar-normal-icon: url(undock.png);
        }

        QDockWidget::title {
            text-align: left;
            background: rgb(45, 45, 45);
            padding-left: 5px;
        }

        QToolBar {
            background: rgb(53, 53, 53);
            border: 1px solid #444;
            spacing: 3px;
        }

        QToolButton {
            background: rgb(53, 53, 53);
            border: 1px solid #444;
            border-radius: 2px;
        }

        QToolButton:hover {
            background: rgb(60, 60, 60);
        }

        QMenuBar {
            background: rgb(53, 53, 53);
        }

        QMenuBar::item {
            spacing: 3px;
            padding: 3px 10px;
            background: transparent;
        }

        QMenuBar::item:selected {
            background: rgb(45, 45, 45);
        }

        QMenu {
            background: rgb(53, 53, 53);
            border: 1px solid #444;
        }

        QMenu::item {
            padding: 5px 30px 5px 30px;
        }

        QMenu::item:selected {
            background: rgb(45, 45, 45);
        }

        QStatusBar {
            background: rgb(53, 53, 53);
            color: white;
        }
    )");

    wydbr::studio::MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
} 
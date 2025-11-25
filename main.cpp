#include <QApplication>
#include "MainWindow.h"
#include <QMessageBox>

int main(int argc, char *argv[]) {

    // Initialize QT Application, handles system events
    QApplication app(argc, argv);

    // Create Main window
    MainWindow mainWindow;
    mainWindow.setWindowTitle("JobHunter");
    mainWindow.resize(800, 600);
    mainWindow.show();

    return app.exec();
}
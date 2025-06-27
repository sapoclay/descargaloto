#include <QApplication>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QSharedMemory>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Configurar información de la aplicación
    app.setApplicationName("DescargaloTó");
    app.setApplicationVersion("1.0.2");
    app.setOrganizationName("SapoClay");
    app.setApplicationDisplayName("DescargaloTó");
    
    // Verificar si la aplicación ya está ejecutándose
    QSharedMemory sharedMemory("DescargaloTo_SingleInstance");
    if (!sharedMemory.create(1)) {
        QMessageBox::warning(nullptr, "DescargaloTó", 
                           "La aplicación ya está ejecutándose.");
        return 1;
    }
    
    // Verificar soporte para bandeja del sistema
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "DescargaloTó",
                            "No se pudo detectar la bandeja del sistema en este sistema.");
        return 1;
    }
    
    // Establecer el icono de la aplicación
    app.setWindowIcon(QIcon(":/img/logo.png"));
    
    // Crear y mostrar splash screen
    QPixmap pixmap(":/img/logo.png");
    if (!pixmap.isNull()) {
        QSplashScreen splash(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        splash.show();
        
        // Mostrar el splash por 2 segundos
        QTimer::singleShot(2000, &splash, &QSplashScreen::close);
    }
    
    // Crear ventana principal
    MainWindow window;
    
    // No mostrar la ventana inmediatamente si hay soporte para bandeja del sistema
    QApplication::setQuitOnLastWindowClosed(false);
    
    return app.exec();
}

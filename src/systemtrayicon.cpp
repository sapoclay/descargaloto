#include "systemtrayicon.h"
#include <QApplication>

SystemTrayIcon::SystemTrayIcon(QObject *parent)
    : QSystemTrayIcon(parent)
    , m_contextMenu(nullptr)
    , m_showAction(nullptr)
    , m_exitAction(nullptr)
{
    setIcon(QIcon(":/img/logo.png"));
    setToolTip("DescargaloTó - Gestor de Descargas");
    
    setupContextMenu();
    
    // Conectar señales
    connect(this, &QSystemTrayIcon::activated, this, &SystemTrayIcon::onActivated);
}

void SystemTrayIcon::setupContextMenu()
{
    m_contextMenu = new QMenu();
    
    m_showAction = new QAction("🔧 Abrir DescargaloTó", this);
    m_showAction->setIcon(QIcon(":/img/logo.png"));
    connect(m_showAction, &QAction::triggered, this, &SystemTrayIcon::onShowTriggered);
    
    m_contextMenu->addAction(m_showAction);
    m_contextMenu->addSeparator();
    
    m_exitAction = new QAction("❌ Salir", this);
    connect(m_exitAction, &QAction::triggered, this, &SystemTrayIcon::onExitTriggered);
    
    m_contextMenu->addAction(m_exitAction);
    
    // Aplicar estilo al menú contextual
    m_contextMenu->setStyleSheet(R"(
        QMenu {
            background-color: white;
            border: 1px solid #cccccc;
            border-radius: 5px;
            padding: 5px;
            font-size: 13px;
        }
        QMenu::item {
            background-color: transparent;
            padding: 8px 15px;
            border-radius: 3px;
            margin: 1px;
        }
        QMenu::item:selected {
            background-color: #4CAF50;
            color: white;
        }
        QMenu::separator {
            height: 1px;
            background-color: #e0e0e0;
            margin: 5px 10px;
        }
    )");
    
    setContextMenu(m_contextMenu);
}

void SystemTrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        emit showWindow();
        break;
    case QSystemTrayIcon::MiddleClick:
        // Podrías agregar funcionalidad adicional aquí
        break;
    default:
        break;
    }
}

void SystemTrayIcon::onShowTriggered()
{
    emit showWindow();
}

void SystemTrayIcon::onExitTriggered()
{
    emit exitApplication();
}

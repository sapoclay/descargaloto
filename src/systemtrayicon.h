#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

public:
    explicit SystemTrayIcon(QObject *parent = nullptr);

signals:
    void showWindow();
    void exitApplication();

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowTriggered();
    void onExitTriggered();

private:
    void setupContextMenu();
    
    QMenu *m_contextMenu;
    QAction *m_showAction;
    QAction *m_exitAction;
};

#endif // SYSTEMTRAYICON_H

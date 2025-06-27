#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QProgressBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QSplitter>
#include <QTextEdit>
#include <QGroupBox>
#include <QApplication>
#include <QProcess>
#include "preferencesdialog.h"

class DownloadManager;
class AboutDialog;
class SystemTrayIcon;
class PreferencesDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void addDownload();
    void startDownload();
    void removeFromHistory();
    void clearHistory();
    void showAbout();
    void showPreferences();
    void showFromTray();
    void exitApplication();
    void onDownloadProgress(int percentage);
    void onDownloadFinished(const QString &filePath, bool success);
    void onHistorySelectionChanged();
    void openFileLocation(const QString &filePath);
    void onThemeChanged(PreferencesDialog::Theme theme);

private:
    void setupUI();
    void setupMenuBar();
    void setupSystemTray();
    void loadHistory();
    void saveHistory();
    void updateHistoryTable();
    void setStyleSheet();
    void setLightTheme();
    void setDarkTheme();
    void applyTheme(int theme);
    void initializeSettings();
    void migrateHistoryToCustomDir(const QString &newDir);
    QString getCustomConfigDir() const;
    QString getHistoryFilePath() const;
    
    // UI Components
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QSplitter *m_splitter;
    
    // URL input section
    QGroupBox *m_inputGroup;
    QLineEdit *m_urlLineEdit;
    QPushButton *m_addButton;
    QPushButton *m_downloadButton;
    
    // Progress section
    QGroupBox *m_progressGroup;
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QTextEdit *m_logTextEdit;
    
    // History section
    QGroupBox *m_historyGroup;
    QTableWidget *m_historyTable;
    QPushButton *m_removeButton;
    QPushButton *m_clearButton;
    
    // Menu actions
    QAction *m_exitAction;
    QAction *m_aboutAction;
    QAction *m_preferencesAction;
    
    // System tray
    SystemTrayIcon *m_systemTrayIcon;
    
    // Download manager
    DownloadManager *m_downloadManager;
    
    // Settings
    QSettings *m_settings;
    
    // History data
    QStringList m_downloadHistory;
    
    // Current download info
    QString m_currentDownloadPath;
    
    // Current theme
    int m_currentTheme;
};

#endif // MAINWINDOW_H

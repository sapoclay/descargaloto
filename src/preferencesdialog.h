#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>
#include <QSettings>
#include <QLineEdit>
#include <QLabel>
#include <QFileDialog>

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

    enum Theme {
        LightTheme = 0,
        DarkTheme = 1
    };

    Theme getCurrentTheme() const;
    void setCurrentTheme(Theme theme);
    
    QString getCustomConfigDir() const;
    void setCustomConfigDir(const QString &dir);

signals:
    void themeChanged(Theme theme);
    void configDirChanged(const QString &newDir);

private slots:
    void onAccept();
    void onReject();
    void onBrowseConfigDir();

private:
    void setupUI();
    void loadSettings();
    void saveSettings();

    // UI Components
    QVBoxLayout *m_mainLayout;
    QGroupBox *m_themeGroup;
    QVBoxLayout *m_themeLayout;
    QRadioButton *m_lightThemeRadio;
    QRadioButton *m_darkThemeRadio;
    QButtonGroup *m_themeButtonGroup;
    
    // Config directory section
    QGroupBox *m_configGroup;
    QVBoxLayout *m_configLayout;
    QHBoxLayout *m_configDirLayout;
    QLabel *m_configDirLabel;
    QLineEdit *m_configDirEdit;
    QPushButton *m_browseButton;
    
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    
    // Settings
    QSettings *m_settings;
    Theme m_currentTheme;
    Theme m_originalTheme;
    QString m_customConfigDir;
    QString m_originalConfigDir;
};

#endif // PREFERENCESDIALOG_H

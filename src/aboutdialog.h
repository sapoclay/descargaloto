#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    enum Theme {
        Light,
        Dark
    };
    
    explicit AboutDialog(QWidget *parent = nullptr);
    void setTheme(Theme theme);

private slots:
    void openGitHubRepository();

private:
    void setupUI();
    void applyTheme();
    
    Theme m_theme = Light;
    
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_iconLayout;
    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
    QLabel *m_versionLabel;
    QTextEdit *m_descriptionText;
    QHBoxLayout *m_buttonLayout;
    QPushButton *m_githubButton;
    QPushButton *m_closeButton;
};

#endif // ABOUTDIALOG_H
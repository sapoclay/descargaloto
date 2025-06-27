#include "aboutdialog.h"
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QIcon>
#include <QPixmap>
#include <QDesktopServices>
#include <QUrl>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Acerca de DescargaloTó");
    setWindowIcon(QIcon(":/img/logo.png"));
    setFixedSize(500, 400);
    setModal(true);
    setupUI();
    applyTheme();
    connect(m_githubButton, &QPushButton::clicked, this, &AboutDialog::openGitHubRepository);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

void AboutDialog::setTheme(Theme theme)
{
    m_theme = theme;
    applyTheme();
}

void AboutDialog::applyTheme()
{
    if (m_theme == Dark) {
        setStyleSheet(R"(
            QDialog { background-color: #232629; color: #ffffff; }
            QLabel, QTextEdit { color: #ffffff; }
            QTextEdit { background-color: #2b2b2b; border: 1px solid #444; }
            QPushButton { background-color: #3498db; color: #fff; }
            QPushButton:hover { background-color: #2980b9; }
            QPushButton:pressed { background-color: #21618c; }
        )");
        m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #ffffff;");
        m_versionLabel->setStyleSheet("font-size: 14px; color: #bbbbbb;");
    } else {
        setStyleSheet(R"(
            QDialog { background-color: #ffffff; color: #222222; }
            QLabel, QTextEdit { color: #222222; }
            QTextEdit { background-color: #ecf0f1; border: 1px solid #bdc3c7; }
            QPushButton { background-color: #3498db; color: #fff; }
            QPushButton:hover { background-color: #2980b9; }
            QPushButton:pressed { background-color: #21618c; }
        )");
        m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");
        m_versionLabel->setStyleSheet("font-size: 14px; color: #7f8c8d;");
    }
}

void AboutDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(30, 30, 30, 30);
    m_iconLayout = new QHBoxLayout;
    m_iconLabel = new QLabel;
    QPixmap icon(":/img/logo.png");
    if (!icon.isNull()) {
        m_iconLabel->setPixmap(icon.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_iconLabel->setText("📱");
        m_iconLabel->setStyleSheet("font-size: 64px;");
    }
    m_iconLabel->setAlignment(Qt::AlignCenter);
    auto infoLayout = new QVBoxLayout;
    m_titleLabel = new QLabel("DescargaloTó");
    m_versionLabel = new QLabel("Versión 1.0.3");
    infoLayout->addWidget(m_titleLabel);
    infoLayout->addWidget(m_versionLabel);
    infoLayout->addStretch();
    m_iconLayout->addWidget(m_iconLabel);
    m_iconLayout->addLayout(infoLayout);
    m_iconLayout->addStretch();
    m_mainLayout->addLayout(m_iconLayout);
    m_descriptionText = new QTextEdit;
    m_descriptionText->setReadOnly(true);
    m_descriptionText->setMaximumHeight(180);
    QString description = R"(
<h3>¿Qué hace DescargaloTó?</h3>
<p><strong>DescargaloTó</strong> es un gestor de descargas multiplataforma diseñado para Linux y Windows que te permite:</p>
<ul>
<li>📥 <strong>Descargar archivos</strong> desde cualquier URL de internet</li>
<li>📂 <strong>Elegir la carpeta</strong> y el nombre del archivo de destino</li>
<li>📊 <strong>Ver el progreso</strong> de descarga en tiempo real</li>
<li>📚 <strong>Mantener un historial</strong> de URLs para reutilizarlas fácilmente</li>
<li>🗑️ <strong>Gestionar el historial</strong> añadiendo y eliminando URLs</li>
<li>📱 <strong>Ejecutarse en la bandeja del sistema</strong> para un acceso rápido</li>
<li>🎨 <strong>Interfaz moderna y fácil de usar</strong></li>
</ul>
<p>Perfecto para gestionar tus descargas de forma eficiente y organizada.</p>
)";
    m_descriptionText->setHtml(description);
    m_mainLayout->addWidget(m_descriptionText);
    m_buttonLayout = new QHBoxLayout;
    m_githubButton = new QPushButton("Ver en GitHub");
    m_githubButton->setIcon(QIcon(":/img/github.svg"));
    m_githubButton->setIconSize(QSize(20,20));
    m_closeButton = new QPushButton("Cerrar");
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_githubButton);
    m_buttonLayout->addWidget(m_closeButton);
    m_mainLayout->addLayout(m_buttonLayout);
}

void AboutDialog::openGitHubRepository()
{
    QDesktopServices::openUrl(QUrl("https://github.com/sapoclay/descargaloto"));
}
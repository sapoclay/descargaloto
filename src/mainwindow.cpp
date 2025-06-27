#include "mainwindow.h"
#include "downloadmanager.h"
#include "aboutdialog.h"
#include "systemtrayicon.h"
#include <QApplication>
#include <QHeaderView>
#include <QDir>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_downloadManager(new DownloadManager(this))
    , m_systemTrayIcon(nullptr)
    , m_settings(nullptr)
    , m_currentTheme(0)
{
    initializeSettings();
    
    setupUI();
    setupMenuBar();
    setupSystemTray();
    loadHistory();
    updateHistoryTable();
    
    // Conectar señales del download manager
    connect(m_downloadManager, &DownloadManager::downloadProgress,
            this, &MainWindow::onDownloadProgress);
    connect(m_downloadManager, &DownloadManager::downloadFinished,
            this, &MainWindow::onDownloadFinished);
    
    // Conectar señales de la interfaz
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::addDownload);
    connect(m_downloadButton, &QPushButton::clicked, this, &MainWindow::startDownload);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::removeFromHistory);
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::clearHistory);
    connect(m_historyTable, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onHistorySelectionChanged);
    connect(m_urlLineEdit, &QLineEdit::returnPressed, this, &MainWindow::addDownload);
    
    // Aplicar tema guardado
    m_currentTheme = m_settings->value("theme", 0).toInt();
    
    // Aplicar tema
    applyTheme(m_currentTheme);
    
    setWindowTitle("DescargaloTó - Gestor de Descargas");
    setWindowIcon(QIcon(":/img/logo.png"));
    resize(800, 600);
    
    // Configurar la tabla inicial
    onHistorySelectionChanged();
}

MainWindow::~MainWindow()
{
    if (m_systemTrayIcon) {
        m_systemTrayIcon->hide();
    }
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_splitter = new QSplitter(Qt::Vertical);
    
    // Sección de entrada de URL
    m_inputGroup = new QGroupBox("Nueva Descarga");
    auto inputLayout = new QHBoxLayout(m_inputGroup);
    
    m_urlLineEdit = new QLineEdit;
    m_urlLineEdit->setPlaceholderText("Introduce una URL para añadir al historial...");
    m_addButton = new QPushButton("Añadir al Historial");
    
    inputLayout->addWidget(m_urlLineEdit);
    inputLayout->addWidget(m_addButton);
    
    // Sección de progreso
    m_progressGroup = new QGroupBox("Estado de Descarga");
    auto progressLayout = new QVBoxLayout(m_progressGroup);
    
    m_statusLabel = new QLabel("Listo para descargar");
    m_progressBar = new QProgressBar;
    m_progressBar->setVisible(false);
    
    m_logTextEdit = new QTextEdit;
    m_logTextEdit->setMaximumHeight(100);
    m_logTextEdit->setReadOnly(true);
    
    progressLayout->addWidget(m_statusLabel);
    progressLayout->addWidget(m_progressBar);
    progressLayout->addWidget(m_logTextEdit);
    
    // Sección de historial
    m_historyGroup = new QGroupBox("Historial de Descargas");
    auto historyLayout = new QVBoxLayout(m_historyGroup);
    
    m_historyTable = new QTableWidget;
    m_historyTable->setColumnCount(2);
    m_historyTable->setHorizontalHeaderLabels({"URL", "Fecha"});
    m_historyTable->horizontalHeader()->setStretchLastSection(true);
    m_historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_historyTable->setAlternatingRowColors(true);
    
    auto buttonLayout = new QHBoxLayout;
    m_downloadButton = new QPushButton("Descargar Ahora");
    m_downloadButton->setEnabled(false);
    m_removeButton = new QPushButton("Eliminar Seleccionada");
    m_removeButton->setEnabled(false);
    m_clearButton = new QPushButton("Limpiar Historial");
    
    buttonLayout->addWidget(m_downloadButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addWidget(m_clearButton);
    
    historyLayout->addWidget(m_historyTable);
    historyLayout->addLayout(buttonLayout);
    
    // Añadir al splitter
    m_splitter->addWidget(m_inputGroup);
    m_splitter->addWidget(m_progressGroup);
    m_splitter->addWidget(m_historyGroup);
    
    // Configurar proporciones del splitter
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 0);
    m_splitter->setStretchFactor(2, 1);
    
    m_mainLayout->addWidget(m_splitter);
}

void MainWindow::setupMenuBar()
{
    auto optionsMenu = menuBar()->addMenu("Opciones");
    
    m_preferencesAction = optionsMenu->addAction("Preferencias...");
    connect(m_preferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);
    
    optionsMenu->addSeparator();
    
    m_aboutAction = optionsMenu->addAction("Acerca de...");
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    
    optionsMenu->addSeparator();
    
    m_exitAction = optionsMenu->addAction("Salir");
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::exitApplication);
}

void MainWindow::setupSystemTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }
    
    m_systemTrayIcon = new SystemTrayIcon(this);
    m_systemTrayIcon->show();
    
    connect(m_systemTrayIcon, &SystemTrayIcon::showWindow, this, &MainWindow::showFromTray);
    connect(m_systemTrayIcon, &SystemTrayIcon::exitApplication, this, &MainWindow::exitApplication);
}

void MainWindow::addDownload()
{
    QString url = m_urlLineEdit->text().trimmed();
    if (url.isEmpty()) {
        return;
    }
    
    if (!m_downloadHistory.contains(url)) {
        m_downloadHistory.append(url);
        saveHistory();
        updateHistoryTable();
        
        m_logTextEdit->append(QString("✅ URL añadida al historial: %1").arg(url));
    } else {
        m_logTextEdit->append(QString("⚠️ La URL ya existe en el historial: %1").arg(url));
    }
    
    m_urlLineEdit->clear();
}

void MainWindow::startDownload()
{
    int row = m_historyTable->currentRow();
    if (row < 0) {
        return;
    }
    
    QString url = m_downloadHistory.at(row);
    
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString selectedDir = QFileDialog::getExistingDirectory(this, "Seleccionar carpeta de destino", downloadsPath);
    
    if (selectedDir.isEmpty()) {
        return;
    }
    
    QString fileName = QInputDialog::getText(this, "Nombre del archivo", 
                                           "Introduce el nombre del archivo:", 
                                           QLineEdit::Normal, 
                                           QUrl(url).fileName());
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QString filePath = QDir(selectedDir).filePath(fileName);
    m_currentDownloadPath = filePath;
    
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    m_statusLabel->setText("Iniciando descarga...");
    
    m_logTextEdit->append(QString("🚀 Iniciando descarga: %1").arg(url));
    m_logTextEdit->append(QString("📁 Destino: %1").arg(filePath));
    
    m_downloadManager->startDownload(url, filePath);
}

void MainWindow::removeFromHistory()
{
    int row = m_historyTable->currentRow();
    if (row >= 0 && row < m_downloadHistory.size()) {
        QString removedUrl = m_downloadHistory.at(row);
        m_downloadHistory.removeAt(row);
        saveHistory();
        updateHistoryTable();
        
        m_logTextEdit->append(QString("🗑️ URL eliminada del historial: %1").arg(removedUrl));
        
        onHistorySelectionChanged();
    }
}

void MainWindow::clearHistory()
{
    if (m_downloadHistory.isEmpty()) {
        return;
    }
    
    auto reply = QMessageBox::question(this, "Confirmar", 
                                     "¿Estás seguro de que quieres limpiar todo el historial?",
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        m_downloadHistory.clear();
        saveHistory();
        updateHistoryTable();
        
        m_logTextEdit->append("🧹 Historial limpiado completamente");
        
        onHistorySelectionChanged();
    }
}

void MainWindow::showAbout()
{
    AboutDialog dialog(this);
    
    // Aplicar el tema actual al diálogo About
    AboutDialog::Theme aboutTheme = (m_currentTheme == 1) ? AboutDialog::Dark : AboutDialog::Light;
    dialog.setTheme(aboutTheme);
    
    dialog.exec();
}

void MainWindow::showPreferences()
{
    // Crear un nuevo diálogo cada vez para evitar problemas de estado
    PreferencesDialog dialog(this);
    dialog.setCurrentTheme(static_cast<PreferencesDialog::Theme>(m_currentTheme));
    
    // Configurar directorio personalizado si existe
    QString customDir = getCustomConfigDir();
    if (!customDir.isEmpty()) {
        dialog.setCustomConfigDir(customDir);
    }
    
    connect(&dialog, &PreferencesDialog::themeChanged, this, &MainWindow::onThemeChanged);
    connect(&dialog, &PreferencesDialog::configDirChanged, this, &MainWindow::migrateHistoryToCustomDir);
    
    dialog.exec();
}

void MainWindow::showFromTray()
{
    show();
    raise();
    activateWindow();
}

void MainWindow::exitApplication()
{
    QApplication::quit();
}

void MainWindow::onThemeChanged(PreferencesDialog::Theme theme)
{
    m_currentTheme = static_cast<int>(theme);
    applyTheme(m_currentTheme);
    
    // Guardar preferencia
    m_settings->setValue("theme", m_currentTheme);
    m_settings->sync();
}

void MainWindow::onDownloadProgress(int percentage)
{
    m_progressBar->setValue(percentage);
    m_statusLabel->setText(QString("Descargando... %1%").arg(percentage));
}

void MainWindow::onDownloadFinished(const QString &filePath, bool success)
{
    m_progressBar->setVisible(false);
    
    if (success) {
        m_statusLabel->setText("Descarga completada exitosamente");
        m_logTextEdit->append(QString("✅ Descarga completada: %1").arg(filePath));
        
        auto reply = QMessageBox::question(this, "Descarga Completada",
                                         QString("Descarga completada exitosamente.\n\n¿Deseas abrir la carpeta que contiene el archivo?"),
                                         QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            openFileLocation(filePath);
        }
    } else {
        m_statusLabel->setText("Error en la descarga");
        m_logTextEdit->append(QString("❌ Error en la descarga: %1").arg(filePath));
        
        QMessageBox::warning(this, "Error de Descarga",
                           "Ha ocurrido un error durante la descarga. Por favor, verifica la URL e inténtalo nuevamente.");
    }
}

void MainWindow::onHistorySelectionChanged()
{
    bool hasSelection = m_historyTable->currentRow() >= 0;
    m_downloadButton->setEnabled(hasSelection);
    m_removeButton->setEnabled(hasSelection);
}

void MainWindow::openFileLocation(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString dirPath = fileInfo.absolutePath();
    
#ifdef Q_OS_WIN
    QProcess::startDetached("explorer", {"/select,", QDir::toNativeSeparators(filePath)});
#elif defined(Q_OS_MAC)
    QProcess::startDetached("open", {"-R", filePath});
#else
    // Linux y otros Unix-like
    QDesktopServices::openUrl(QUrl::fromLocalFile(dirPath));
#endif
}

void MainWindow::loadHistory()
{
    QString historyStr = m_settings->value("downloadHistory", "").toString();
    if (!historyStr.isEmpty()) {
        m_downloadHistory = historyStr.split(", ", Qt::SkipEmptyParts);
    }
}

void MainWindow::saveHistory()
{
    QString historyStr = m_downloadHistory.join(", ");
    m_settings->setValue("downloadHistory", historyStr);
    m_settings->sync();
}

void MainWindow::updateHistoryTable()
{
    m_historyTable->setRowCount(m_downloadHistory.size());
    
    for (int i = 0; i < m_downloadHistory.size(); ++i) {
        auto urlItem = new QTableWidgetItem(m_downloadHistory.at(i));
        auto dateItem = new QTableWidgetItem(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
        
        m_historyTable->setItem(i, 0, urlItem);
        m_historyTable->setItem(i, 1, dateItem);
    }
    
    // Reajustar columnas
    m_historyTable->resizeColumnToContents(1);
    
    // Aplicar tema actual a nuevos elementos
    applyTheme(m_currentTheme);
}

void MainWindow::applyTheme(int theme)
{
    if (theme == 1) {
        setDarkTheme();
    } else {
        setLightTheme();
    }
}

void MainWindow::setLightTheme()
{
    QWidget::setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
            color: #333333;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #cccccc;
            border-radius: 5px;
            margin-top: 1ex;
            padding-top: 10px;
            background-color: #ffffff;
            color: #333333;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #333333;
        }
        QPushButton {
            background-color: #4CAF50;
            border: none;
            color: white;
            padding: 8px 16px;
            text-align: center;
            font-size: 14px;
            margin: 4px 2px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }
        QLineEdit {
            padding: 8px;
            border: 2px solid #ddd;
            border-radius: 4px;
            font-size: 14px;
            background-color: #ffffff;
            color: #333333;
        }
        QLineEdit:focus {
            border-color: #4CAF50;
        }
        QTableWidget {
            gridline-color: #cccccc;
            background-color: #ffffff;
            selection-background-color: #4CAF50;
            color: #333333;
        }
        QTableWidget::item {
            color: #333333;
            border-bottom: 1px solid #eeeeee;
        }
        QHeaderView::section {
            background-color: #f5f5f5;
            color: #333333;
            border: 1px solid #cccccc;
            padding: 4px;
        }
        QProgressBar {
            border: 2px solid #cccccc;
            border-radius: 5px;
            text-align: center;
            background-color: #f5f5f5;
            color: #333333;
        }
        QProgressBar::chunk {
            background-color: #4CAF50;
            border-radius: 3px;
        }
        QTextEdit {
            background-color: #ffffff;
            color: #333333;
            border: 2px solid #ddd;
            border-radius: 4px;
        }
        QMenuBar {
            background-color: #f5f5f5;
            color: #333333;
        }
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 8px;
        }
        QMenuBar::item:selected {
            background-color: #4CAF50;
            color: white;
        }
        QMenu {
            background-color: #ffffff;
            color: #333333;
            border: 1px solid #cccccc;
        }
        QMenu::item:selected {
            background-color: #4CAF50;
            color: white;
        }
        QStatusBar {
            background-color: #f5f5f5;
            color: #333333;
        }
        QLabel {
            color: #333333;
        }
        QRadioButton {
            color: #333333;
        }
        QDialog {
            background-color: #f5f5f5;
            color: #333333;
        }
    )");
}

void MainWindow::setDarkTheme()
{
    QWidget::setStyleSheet(R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #555555;
            border-radius: 5px;
            margin-top: 1ex;
            padding-top: 10px;
            background-color: #3c3c3c;
            color: #ffffff;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #ffffff;
        }
        QPushButton {
            background-color: #4CAF50;
            border: none;
            color: white;
            padding: 8px 16px;
            text-align: center;
            font-size: 14px;
            margin: 4px 2px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        QPushButton:disabled {
            background-color: #555555;
            color: #888888;
        }
        QLineEdit {
            padding: 8px;
            border: 2px solid #555555;
            border-radius: 4px;
            font-size: 14px;
            background-color: #3c3c3c;
            color: #ffffff;
        }
        QLineEdit:focus {
            border-color: #4CAF50;
        }
        QTableWidget {
            gridline-color: #555555;
            background-color: #3c3c3c;
            selection-background-color: #4CAF50;
            color: #ffffff;
        }
        QTableWidget::item {
            color: #ffffff;
            border-bottom: 1px solid #555555;
        }
        QHeaderView::section {
            background-color: #2b2b2b;
            color: #ffffff;
            border: 1px solid #555555;
            padding: 4px;
        }
        QProgressBar {
            border: 2px solid #555555;
            border-radius: 5px;
            text-align: center;
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QProgressBar::chunk {
            background-color: #4CAF50;
            border-radius: 3px;
        }
        QTextEdit {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 2px solid #555555;
            border-radius: 4px;
        }
        QMenuBar {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 8px;
        }
        QMenuBar::item:selected {
            background-color: #4CAF50;
            color: white;
        }
        QMenu {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
        }
        QMenu::item:selected {
            background-color: #4CAF50;
            color: white;
        }
        QStatusBar {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QLabel {
            color: #ffffff;
        }
        QRadioButton {
            color: #ffffff;
        }
        QDialog {
            background-color: #2b2b2b;
            color: #ffffff;
        }
    )");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_systemTrayIcon && m_systemTrayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::initializeSettings()
{
    // Inicializar QSettings con configuración por defecto
    m_settings = new QSettings("SapoClay", "DescargaloTó", this);
    
    // Verificar si hay configuración de directorio personalizado
    QString customDir = m_settings->value("customConfigDir", "").toString();
    
    if (!customDir.isEmpty()) {
        // Recrear QSettings con el directorio personalizado
        delete m_settings;
        
        QString configFilePath = QDir(customDir).filePath("DescargaloTo_config.ini");
        m_settings = new QSettings(configFilePath, QSettings::IniFormat, this);
    }
}

QString MainWindow::getCustomConfigDir() const
{
    if (m_settings) {
        return m_settings->value("customConfigDir", "").toString();
    }
    return QString();
}

QString MainWindow::getHistoryFilePath() const
{
    QString customDir = getCustomConfigDir();
    if (!customDir.isEmpty()) {
        return QDir(customDir).filePath("DescargaloTo_config.ini");
    }
    
    // Usar directorio por defecto
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + 
           "/SapoClay/DescargaloTó.conf";
}

void MainWindow::migrateHistoryToCustomDir(const QString &newDir)
{
    if (newDir.isEmpty()) {
        return;
    }
    
    // Guardar historial actual antes de migrar
    saveHistory();
    
    // Crear nuevo archivo de configuración
    QString newConfigPath = QDir(newDir).filePath("DescargaloTo_config.ini");
    QSettings newSettings(newConfigPath, QSettings::IniFormat);
    
    // Copiar toda la configuración actual
    if (m_settings) {
        QStringList keys = m_settings->allKeys();
        for (const QString &key : keys) {
            newSettings.setValue(key, m_settings->value(key));
        }
        newSettings.setValue("customConfigDir", newDir);
        newSettings.sync();
        
        // Actualizar la configuración actual
        delete m_settings;
        m_settings = new QSettings(newConfigPath, QSettings::IniFormat, this);
        
        // Aplicar tema actual después de la migración
        applyTheme(m_currentTheme);
    }
}

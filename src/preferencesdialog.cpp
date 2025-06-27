#include "preferencesdialog.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , m_settings(new QSettings(this))
    , m_currentTheme(LightTheme)
    , m_originalTheme(LightTheme)
    , m_customConfigDir("")
    , m_originalConfigDir("")
{
    setupUI();
    loadSettings();
    setWindowTitle("Preferencias - DescargaloTó");
    setModal(true);
    resize(450, 300);
}

PreferencesDialog::~PreferencesDialog()
{
}

void PreferencesDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // Grupo de tema
    m_themeGroup = new QGroupBox("Tema de la aplicación", this);
    m_themeLayout = new QVBoxLayout(m_themeGroup);
    
    // Radio buttons para tema
    m_lightThemeRadio = new QRadioButton("Tema claro", this);
    m_darkThemeRadio = new QRadioButton("Tema oscuro", this);
    
    m_themeButtonGroup = new QButtonGroup(this);
    m_themeButtonGroup->addButton(m_lightThemeRadio, LightTheme);
    m_themeButtonGroup->addButton(m_darkThemeRadio, DarkTheme);
    
    m_themeLayout->addWidget(m_lightThemeRadio);
    m_themeLayout->addWidget(m_darkThemeRadio);
    
    m_mainLayout->addWidget(m_themeGroup);
    
    // Grupo de directorio de configuración
    m_configGroup = new QGroupBox("Directorio de configuración", this);
    m_configLayout = new QVBoxLayout(m_configGroup);
    
    m_configDirLabel = new QLabel("Directorio donde guardar el historial de descargas:", this);
    m_configLayout->addWidget(m_configDirLabel);
    
    m_configDirLayout = new QHBoxLayout();
    m_configDirEdit = new QLineEdit(this);
    m_configDirEdit->setReadOnly(true);
    m_configDirEdit->setPlaceholderText("Usar directorio por defecto del sistema");
    
    m_browseButton = new QPushButton("Examinar...", this);
    m_browseButton->setMaximumWidth(100);
    
    m_configDirLayout->addWidget(m_configDirEdit);
    m_configDirLayout->addWidget(m_browseButton);
    
    m_configLayout->addLayout(m_configDirLayout);
    
    // Añadir nota informativa
    QLabel *noteLabel = new QLabel("Nota: Si cambias el directorio, el historial actual se moverá a la nueva ubicación.", this);
    noteLabel->setWordWrap(true);
    noteLabel->setStyleSheet("QLabel { color: #666; font-size: 10px; }");
    m_configLayout->addWidget(noteLabel);
    
    m_mainLayout->addWidget(m_configGroup);
    
    // Espaciador
    m_mainLayout->addStretch();
    
    // Botones
    m_buttonLayout = new QHBoxLayout();
    m_okButton = new QPushButton("Aceptar", this);
    m_cancelButton = new QPushButton("Cancelar", this);
    
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_okButton);
    m_buttonLayout->addWidget(m_cancelButton);
    
    m_mainLayout->addLayout(m_buttonLayout);
    
    // Conectar señales
    connect(m_okButton, &QPushButton::clicked, this, &PreferencesDialog::onAccept);
    connect(m_cancelButton, &QPushButton::clicked, this, &PreferencesDialog::onReject);
    connect(m_browseButton, &QPushButton::clicked, this, &PreferencesDialog::onBrowseConfigDir);
}

void PreferencesDialog::loadSettings()
{
    // Cargar tema
    int theme = m_settings->value("theme", LightTheme).toInt();
    m_currentTheme = static_cast<Theme>(theme);
    m_originalTheme = m_currentTheme;
    setCurrentTheme(m_currentTheme);
    
    // Cargar directorio personalizado
    QString customDir = m_settings->value("customConfigDir", "").toString();
    setCustomConfigDir(customDir);
}

void PreferencesDialog::saveSettings()
{
    // Guardar tema
    m_settings->setValue("theme", static_cast<int>(m_currentTheme));
    
    // Verificar si el directorio de configuración cambió
    if (m_customConfigDir != m_originalConfigDir) {
        // Guardar nueva configuración
        m_settings->setValue("customConfigDir", m_customConfigDir);
        
        // Emitir señal de cambio de directorio
        emit configDirChanged(m_customConfigDir);
        
        // Actualizar directorio original
        m_originalConfigDir = m_customConfigDir;
    }
    
    m_settings->sync();
}

PreferencesDialog::Theme PreferencesDialog::getCurrentTheme() const
{
    return m_currentTheme;
}

void PreferencesDialog::setCurrentTheme(Theme theme)
{
    m_currentTheme = theme;
    m_originalTheme = theme;
    
    // Desconectar temporalmente las señales para evitar bucles
    m_themeButtonGroup->blockSignals(true);
    
    if (theme == LightTheme) {
        m_lightThemeRadio->setChecked(true);
    } else {
        m_darkThemeRadio->setChecked(true);
    }
    
    // Reconectar las señales
    m_themeButtonGroup->blockSignals(false);
}

void PreferencesDialog::onAccept()
{
    // Obtener tema seleccionado
    int selectedTheme = m_themeButtonGroup->checkedId();
    if (selectedTheme != -1) {
        Theme newTheme = static_cast<Theme>(selectedTheme);
        
        // Actualizar tema actual
        m_currentTheme = newTheme;
        
        // Siempre emitir la señal para actualizar la interfaz
        // (incluso si es el mismo tema, para asegurar consistencia)
        saveSettings();
        emit themeChanged(newTheme);
    }
    
    // Verificar cambio de directorio
    if (m_customConfigDir != m_originalConfigDir) {
        int ret = QMessageBox::question(this, "Cambiar directorio",
                                      QString("¿Deseas mover el historial actual al nuevo directorio?\n\n"
                                             "Nuevo directorio: %1").arg(m_customConfigDir.isEmpty() ? 
                                             "Directorio por defecto del sistema" : m_customConfigDir),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        
        if (ret == QMessageBox::Cancel) {
            return; // No cerrar el diálogo
        }
        
        if (ret == QMessageBox::Yes) {
            saveSettings(); // Esto emitirá configDirChanged
        } else {
            // Solo cambiar sin migrar
            m_settings->setValue("customConfigDir", m_customConfigDir);
            m_settings->sync();
            emit configDirChanged(m_customConfigDir);
        }
    }
    
    accept();
}

void PreferencesDialog::onReject()
{
    // Restaurar tema original
    setCurrentTheme(m_originalTheme);
    reject();
}

void PreferencesDialog::onBrowseConfigDir()
{
    QString currentDir = m_configDirEdit->text();
    if (currentDir.isEmpty()) {
        currentDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    }
    
    QString newDir = QFileDialog::getExistingDirectory(this, 
                                                      "Seleccionar directorio de configuración",
                                                      currentDir);
    if (!newDir.isEmpty()) {
        m_configDirEdit->setText(newDir);
        m_customConfigDir = newDir;
    }
}

QString PreferencesDialog::getCustomConfigDir() const
{
    return m_customConfigDir;
}

void PreferencesDialog::setCustomConfigDir(const QString &dir)
{
    m_customConfigDir = dir;
    m_originalConfigDir = dir;
    m_configDirEdit->setText(dir);
}

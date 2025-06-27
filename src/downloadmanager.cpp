#include "downloadmanager.h"
#include <QNetworkRequest>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
    , m_currentFile(nullptr)
    , m_totalBytes(0)
    , m_receivedBytes(0)
    , m_isDownloading(false)
{
    // Configurar el network manager
    m_networkManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

DownloadManager::~DownloadManager()
{
    cancelDownload();
}

void DownloadManager::startDownload(const QString &url, const QString &filePath)
{
    if (m_isDownloading) {
        cancelDownload();
    }
    
    m_currentFilePath = filePath;
    
    // Crear directorio si no existe
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            emit downloadError("No se pudo crear el directorio de destino");
            return;
        }
    }
    
    // Crear archivo de destino
    m_currentFile = new QFile(filePath, this);
    if (!m_currentFile->open(QIODevice::WriteOnly)) {
        emit downloadError("No se pudo crear el archivo de destino");
        delete m_currentFile;
        m_currentFile = nullptr;
        return;
    }
    
    // Crear la solicitud de descarga
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, 
                        QNetworkRequest::NoLessSafeRedirectPolicy);
    
    // Agregar headers para simular un navegador
    request.setRawHeader("User-Agent", 
                        "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
                        "(KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36");
    request.setRawHeader("Accept", "*/*");
    request.setRawHeader("Accept-Language", "es-ES,es;q=0.9,en;q=0.8");
    request.setRawHeader("Connection", "keep-alive");
    
    // Iniciar la descarga
    m_currentReply = m_networkManager->get(request);
    
    // Conectar señales
    connect(m_currentReply, &QNetworkReply::downloadProgress,
            this, &DownloadManager::onDownloadProgress);
    connect(m_currentReply, &QNetworkReply::finished,
            this, &DownloadManager::onDownloadFinished);
    connect(m_currentReply, &QNetworkReply::errorOccurred,
            this, &DownloadManager::onDownloadError);
    connect(m_currentReply, &QNetworkReply::readyRead,
            this, &DownloadManager::onReadyRead);
    
    m_isDownloading = true;
    m_totalBytes = 0;
    m_receivedBytes = 0;
    
    emit downloadProgress(0);
}

void DownloadManager::cancelDownload()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
    
    if (m_currentFile) {
        m_currentFile->close();
        m_currentFile->deleteLater();
        m_currentFile = nullptr;
    }
    
    m_isDownloading = false;
}

bool DownloadManager::isDownloading() const
{
    return m_isDownloading;
}

void DownloadManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        m_totalBytes = bytesTotal;
        m_receivedBytes = bytesReceived;
        
        int percentage = static_cast<int>((bytesReceived * 100) / bytesTotal);
        emit downloadProgress(percentage);
    }
}

void DownloadManager::onDownloadFinished()
{
    m_isDownloading = false;
    
    if (!m_currentReply) {
        return;
    }
    
    bool success = false;
    QString errorString;
    
    if (m_currentReply->error() == QNetworkReply::NoError) {
        // Leer los datos restantes
        if (m_currentFile) {
            QByteArray data = m_currentReply->readAll();
            if (!data.isEmpty()) {
                m_currentFile->write(data);
            }
            m_currentFile->flush();
            m_currentFile->close();
        }
        success = true;
    } else {
        errorString = m_currentReply->errorString();
    }
    
    // Limpiar
    QString filePath = m_currentFilePath;
    
    if (m_currentFile) {
        m_currentFile->deleteLater();
        m_currentFile = nullptr;
    }
    
    if (m_currentReply) {
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
    
    // Si hubo error, eliminar archivo parcial
    if (!success && QFile::exists(filePath)) {
        QFile::remove(filePath);
    }
    
    // Emitir señal de finalización
    if (success) {
        emit downloadFinished(filePath, true);
    } else {
        emit downloadError(errorString);
        emit downloadFinished(filePath, false);
    }
}

void DownloadManager::onDownloadError(QNetworkReply::NetworkError error)
{
    QString errorString;
    
    switch (error) {
    case QNetworkReply::ConnectionRefusedError:
        errorString = "Conexión rechazada";
        break;
    case QNetworkReply::RemoteHostClosedError:
        errorString = "El servidor cerró la conexión";
        break;
    case QNetworkReply::HostNotFoundError:
        errorString = "Servidor no encontrado";
        break;
    case QNetworkReply::TimeoutError:
        errorString = "Tiempo de espera agotado";
        break;
    case QNetworkReply::OperationCanceledError:
        errorString = "Descarga cancelada";
        break;
    case QNetworkReply::SslHandshakeFailedError:
        errorString = "Error de SSL";
        break;
    case QNetworkReply::TemporaryNetworkFailureError:
        errorString = "Fallo temporal de red";
        break;
    case QNetworkReply::NetworkSessionFailedError:
        errorString = "Fallo de sesión de red";
        break;
    case QNetworkReply::BackgroundRequestNotAllowedError:
        errorString = "Solicitud en segundo plano no permitida";
        break;
    case QNetworkReply::TooManyRedirectsError:
        errorString = "Demasiadas redirecciones";
        break;
    case QNetworkReply::InsecureRedirectError:
        errorString = "Redirección insegura";
        break;
    case QNetworkReply::ProxyConnectionRefusedError:
        errorString = "Conexión proxy rechazada";
        break;
    case QNetworkReply::ProxyConnectionClosedError:
        errorString = "Conexión proxy cerrada";
        break;
    case QNetworkReply::ProxyNotFoundError:
        errorString = "Proxy no encontrado";
        break;
    case QNetworkReply::ProxyTimeoutError:
        errorString = "Tiempo de espera del proxy agotado";
        break;
    case QNetworkReply::ProxyAuthenticationRequiredError:
        errorString = "Autenticación de proxy requerida";
        break;
    case QNetworkReply::ContentAccessDenied:
        errorString = "Acceso al contenido denegado";
        break;
    case QNetworkReply::ContentOperationNotPermittedError:
        errorString = "Operación de contenido no permitida";
        break;
    case QNetworkReply::ContentNotFoundError:
        errorString = "Contenido no encontrado (404)";
        break;
    case QNetworkReply::AuthenticationRequiredError:
        errorString = "Autenticación requerida";
        break;
    case QNetworkReply::ContentReSendError:
        errorString = "Error de reenvío de contenido";
        break;
    case QNetworkReply::ContentConflictError:
        errorString = "Conflicto de contenido";
        break;
    case QNetworkReply::ContentGoneError:
        errorString = "Contenido no disponible";
        break;
    case QNetworkReply::InternalServerError:
        errorString = "Error interno del servidor (500)";
        break;
    case QNetworkReply::OperationNotImplementedError:
        errorString = "Operación no implementada";
        break;
    case QNetworkReply::ServiceUnavailableError:
        errorString = "Servicio no disponible";
        break;
    case QNetworkReply::ProtocolUnknownError:
        errorString = "Protocolo desconocido";
        break;
    case QNetworkReply::ProtocolInvalidOperationError:
        errorString = "Operación de protocolo inválida";
        break;
    case QNetworkReply::UnknownNetworkError:
        errorString = "Error de red desconocido";
        break;
    case QNetworkReply::UnknownProxyError:
        errorString = "Error de proxy desconocido";
        break;
    case QNetworkReply::UnknownContentError:
        errorString = "Error de contenido desconocido";
        break;
    case QNetworkReply::ProtocolFailure:
        errorString = "Fallo de protocolo";
        break;
    case QNetworkReply::UnknownServerError:
        errorString = "Error de servidor desconocido";
        break;
    default:
        errorString = QString("Error de red (%1)").arg(static_cast<int>(error));
        break;
    }
    
    emit downloadError(errorString);
}

void DownloadManager::onReadyRead()
{
    if (m_currentFile && m_currentReply) {
        QByteArray data = m_currentReply->readAll();
        m_currentFile->write(data);
    }
}

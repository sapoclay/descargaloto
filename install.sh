#!/bin/bash

# Script de instalación para DescargaloTó
# Instala el programa en el sistema

set -e

# Colores
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Verificar que el ejecutable existe
if [ ! -f "build/DescargaloTo" ]; then
    print_error "El ejecutable no existe. Ejecuta ./build.sh primero."
    exit 1
fi

# Verificar permisos de administrador
if [ "$EUID" -ne 0 ]; then
    print_warning "Se necesitan permisos de administrador para instalar."
    print_status "Ejecutando con sudo..."
    exec sudo "$0" "$@"
fi

print_status "Instalando DescargaloTó..."

# Crear directorios necesarios
mkdir -p /usr/local/bin
mkdir -p /usr/local/share/applications
mkdir -p /usr/local/share/icons/hicolor/128x128/apps
mkdir -p /usr/local/share/pixmaps

# Copiar ejecutable
print_status "Copiando ejecutable..."
cp build/DescargaloTo /usr/local/bin/
chmod +x /usr/local/bin/DescargaloTo

# Crear wrapper script que soluciona problemas con Snap
print_status "Creando script de ejecución..."
cat > /usr/local/bin/descargaloto-safe << 'EOF'
#!/bin/bash
# Wrapper para DescargaloTó que soluciona problemas con librerías
exec env -u LD_LIBRARY_PATH -u LD_PRELOAD QT_QPA_PLATFORM_PLUGIN_PATH="" /usr/local/bin/DescargaloTo "$@"
EOF
chmod +x /usr/local/bin/descargaloto-safe

# Copiar icono
print_status "Copiando icono..."
if [ -f "img/logo.png" ]; then
    cp img/logo.png /usr/local/share/icons/hicolor/128x128/apps/descargaloto.png
    cp img/logo.png /usr/local/share/pixmaps/descargaloto.png
fi

# Crear archivo .desktop
print_status "Creando entrada del menú de aplicaciones..."
cat > /usr/local/share/applications/descargaloto.desktop << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=DescargaloTó
Comment=Gestor de descargas multiplataforma
Comment[en]=Cross-platform download manager
Exec=env -u LD_LIBRARY_PATH -u LD_PRELOAD QT_QPA_PLATFORM_PLUGIN_PATH="" /usr/local/bin/DescargaloTo
Icon=descargaloto
Terminal=false
Categories=Network;FileTransfer;
StartupWMClass=DescargaloTo
StartupNotify=true
MimeType=x-scheme-handler/http;x-scheme-handler/https;
Keywords=download;manager;files;internet;
EOF

# Hacer ejecutable el archivo .desktop
chmod +x /usr/local/share/applications/descargaloto.desktop

# Actualizar base de datos de aplicaciones
if command -v update-desktop-database &> /dev/null; then
    print_status "Actualizando base de datos de aplicaciones..."
    update-desktop-database /usr/local/share/applications
fi

# Actualizar cache de iconos
if command -v gtk-update-icon-cache &> /dev/null; then
    print_status "Actualizando cache de iconos..."
    gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor
fi

print_status "✅ DescargaloTó instalado correctamente!"
print_status "Puedes ejecutarlo desde:"
echo "  - Menú de aplicaciones: DescargaloTó"
echo "  - Terminal: DescargaloTo"
echo "  - Terminal (seguro): descargaloto-safe"
echo "  - Comando directo: /usr/local/bin/DescargaloTo"
echo ""
print_status "Si tienes problemas con librerías de Snap, usa:"
echo "  descargaloto-safe"

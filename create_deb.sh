#!/bin/bash

# Script para crear el paquete .deb de DescargaloTó
# Automatiza todo el proceso de empaquetado

set -e

# Colores para output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
BLUE='\033[0;34m'
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

print_step() {
    echo -e "${BLUE}[STEP]${NC} $1"
}

# Variables
PACKAGE_NAME="descargaloto"
VERSION="1.0.3"
ARCHITECTURE="amd64"
DEB_FILE="${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb"
BUILD_DIR="build"
PACKAGE_DIR="package"

print_status "🚀 Iniciando creación del paquete .deb para DescargaloTó"

# Verificar que estamos en el directorio correcto
if [ ! -f "CMakeLists.txt" ] || [ ! -f "Makefile" ]; then
    print_error "❌ Debes ejecutar este script desde el directorio raíz del proyecto"
    exit 1
fi

# Paso 1: Compilar el proyecto
print_step "1️⃣  Compilando el proyecto..."
if [ ! -f "$BUILD_DIR/DescargaloTo" ]; then
    print_status "Compilando DescargaloTó..."
    make clean
    make build
else
    print_status "El ejecutable ya existe, omitiendo compilación"
fi

# Verificar que el ejecutable existe
if [ ! -f "$BUILD_DIR/DescargaloTo" ]; then
    print_error "❌ No se pudo crear el ejecutable"
    exit 1
fi

# Paso 2: Limpiar directorio de paquete anterior
print_step "2️⃣  Preparando directorio de empaquetado..."
if [ -d "$PACKAGE_DIR" ]; then
    print_status "Limpiando directorio de paquete anterior..."
    rm -rf "$PACKAGE_DIR"
fi

# Recrear estructura de directorios
mkdir -p "$PACKAGE_DIR"/{DEBIAN,usr/local/{bin,share/{applications,icons/hicolor/128x128/apps,pixmaps,doc/descargaloto}}}

# Paso 3: Copiar ejecutable
print_step "3️⃣  Copiando ejecutable..."
# Renombrar el ejecutable original
cp "$BUILD_DIR/DescargaloTo" "$PACKAGE_DIR/usr/local/bin/DescargaloTo-bin"
chmod +x "$PACKAGE_DIR/usr/local/bin/DescargaloTo-bin"

# Crear wrapper para el comando principal
cat > "$PACKAGE_DIR/usr/local/bin/DescargaloTo" << 'EOF'
#!/bin/bash
# Wrapper principal para DescargaloTó que detecta el entorno automáticamente

# Verificar si hay conflictos de Snap
if [[ -n "$SNAP" ]] || [[ "$PATH" == *"/snap/"* ]] || [[ -n "$LD_LIBRARY_PATH" && "$LD_LIBRARY_PATH" == *"snap"* ]]; then
    # Entorno con Snap detectado, usar método ultra-limpio
    exec env -i \
        HOME="$HOME" \
        USER="$USER" \
        PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
        DISPLAY="${DISPLAY:-:0}" \
        XAUTHORITY="${XAUTHORITY}" \
        XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR}" \
        XDG_SESSION_TYPE="x11" \
        QT_QPA_PLATFORM="xcb" \
        QT_QPA_PLATFORM_PLUGIN_PATH="" \
        /usr/local/bin/DescargaloTo-bin "$@"
else
    # Entorno limpio, ejecutar normalmente
    exec /usr/local/bin/DescargaloTo-bin "$@"
fi
EOF
chmod +x "$PACKAGE_DIR/usr/local/bin/DescargaloTo"

# Paso 4: Copiar script wrapper
print_step "4️⃣  Copiando script wrapper..."
cat > "$PACKAGE_DIR/usr/local/bin/descargaloto-safe" << 'EOF'
#!/bin/bash
# Wrapper para DescargaloTó que soluciona problemas con librerías de Snap
# Este script crea un entorno completamente limpio para evitar conflictos

echo "🚀 Iniciando DescargaloTó con entorno ultra-limpio..."

# Crear un entorno completamente nuevo sin variables de Snap
exec env -i \
    HOME="$HOME" \
    USER="$USER" \
    PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
    DISPLAY="${DISPLAY:-:0}" \
    XAUTHORITY="${XAUTHORITY}" \
    XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR}" \
    XDG_SESSION_TYPE="x11" \
    QT_QPA_PLATFORM="xcb" \
    QT_QPA_PLATFORM_PLUGIN_PATH="" \
    /usr/local/bin/DescargaloTo "$@"
EOF
chmod +x "$PACKAGE_DIR/usr/local/bin/descargaloto-safe"

# Paso 5: Copiar icono
print_step "5️⃣  Copiando icono..."
if [ -f "img/logo.png" ]; then
    cp "img/logo.png" "$PACKAGE_DIR/usr/local/share/icons/hicolor/128x128/apps/descargaloto.png"
    cp "img/logo.png" "$PACKAGE_DIR/usr/local/share/pixmaps/descargaloto.png"
    print_status "✅ Icono copiado"
else
    print_warning "⚠️  No se encontró img/logo.png, el paquete no tendrá icono"
fi

# Paso 6: Crear archivo .desktop
print_step "6️⃣  Creando archivo .desktop..."
cat > "$PACKAGE_DIR/usr/local/share/applications/descargaloto.desktop" << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=DescargaloTó
Name[en]=DescargaloTó
Comment=Gestor de descargas multiplataforma moderno
Comment[en]=Modern cross-platform download manager
Exec=env -u LD_LIBRARY_PATH -u LD_PRELOAD QT_QPA_PLATFORM_PLUGIN_PATH="" /usr/local/bin/DescargaloTo
Icon=descargaloto
Terminal=false
Categories=Network;FileTransfer;Utility;
StartupWMClass=DescargaloTo
StartupNotify=true
MimeType=x-scheme-handler/http;x-scheme-handler/https;
Keywords=download;manager;files;internet;descargas;gestor;archivos;
EOF

# Paso 7: Crear documentación
print_step "7️⃣  Creando documentación..."
cat > "$PACKAGE_DIR/usr/local/share/doc/descargaloto/README" << 'EOF'
DescargaloTó - Gestor de Descargas Multiplataforma
=====================================================

DESCRIPCIÓN
-----------
DescargaloTó es un gestor de descargas moderno desarrollado en C++ con Qt6.
Permite descargar archivos desde URLs de forma sencilla y eficiente con una
interfaz gráfica intuitiva.

CARACTERÍSTICAS
---------------
* Interfaz gráfica moderna e intuitiva
* Gestión de múltiples descargas simultáneas  
* Historial de descargas completo
* Integración con la bandeja del sistema
* Configuración de carpeta de destino
* Manejo robusto de errores
* Temas claro y oscuro personalizables
* Optimizado para sistemas con conflictos de librerías

INSTALACIÓN
-----------
El paquete .deb se instala automáticamente con:
  sudo dpkg -i descargaloto_1.0.0_amd64.deb
  sudo apt-get install -f  # Para resolver dependencias

EJECUCIÓN
---------
Puedes ejecutar DescargaloTó de varias formas:

1. Desde el menú de aplicaciones:
   Busca "DescargaloTó" en el menú

2. Desde terminal:
   DescargaloTo

3. Modo seguro (recomendado si hay problemas con librerías):
   descargaloto-safe

PERSONALIZACIÓN
---------------
El programa incluye opciones de personalización:
- Temas claro y oscuro
- Configuración de carpeta de descargas
- Acceso desde: Opciones → Preferencias

SOLUCIÓN DE PROBLEMAS
--------------------
Si encuentras errores relacionados con librerías (especialmente en sistemas
con Snap instalado), usa el comando 'descargaloto-safe' que limpia el entorno
antes de ejecutar el programa.

Errores comunes:
- "symbol lookup error": Usa descargaloto-safe
- Problemas con Qt plugins: Usa descargaloto-safe

DESINSTALACIÓN
--------------
Para desinstalar:
  sudo apt-get remove descargaloto

SOPORTE
-------
Para reportar errores o solicitar características:
https://github.com/sapoclay/descargaloto

LICENCIA
--------
Consulta el archivo LICENSE incluido con el programa.

AUTOR
-----
DescargaloTó Team
EOF

# Copiar licencia si existe
if [ -f "LICENSE" ]; then
    cp "LICENSE" "$PACKAGE_DIR/usr/local/share/doc/descargaloto/"
fi

# Paso 8: Crear archivos de control DEBIAN
print_step "8️⃣  Creando archivos de control DEBIAN..."

# control
cat > "$PACKAGE_DIR/DEBIAN/control" << EOF
Package: descargaloto
Version: $VERSION
Section: net
Priority: optional
Architecture: amd64
Depends: libqt6core6 (>= 6.0.0), libqt6widgets6 (>= 6.0.0), libqt6network6 (>= 6.0.0), libqt6gui6 (>= 6.0.0), libc6 (>= 2.34), libgcc-s1 (>= 11), libstdc++6 (>= 11)
Installed-Size: 2048
Maintainer: DescargaloTó Team <info@descargaloto.com>
Description: Gestor de descargas multiplataforma moderno
 DescargaloTó es un gestor de descargas desarrollado en C++ con Qt6
 que permite descargar archivos desde URLs de forma sencilla y eficiente.
 .
 Características principales:
  * Interfaz gráfica moderna e intuitiva
  * Gestión de múltiples descargas simultáneas
  * Historial de descargas
  * Integración con la bandeja del sistema
  * Soporte para Linux y Windows
  * Configuración de carpeta de destino
  * Manejo de errores robusto
  * Temas claro y oscuro personalizables
 .
 El programa está optimizado para funcionar correctamente incluso
 en sistemas con conflictos de librerías como Snap/AppImage.
Homepage: https://github.com/sapoclay/descargaloto
EOF

# postinst
cat > "$PACKAGE_DIR/DEBIAN/postinst" << 'EOF'
#!/bin/bash
# Script post-instalación para DescargaloTó

set -e

# Actualizar base de datos de aplicaciones
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database /usr/local/share/applications 2>/dev/null || true
fi

# Actualizar cache de iconos
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor 2>/dev/null || true
fi

# Configurar permisos ejecutables
chmod +x /usr/local/bin/DescargaloTo 2>/dev/null || true
chmod +x /usr/local/bin/descargaloto-safe 2>/dev/null || true

echo "DescargaloTó se ha instalado correctamente."
echo "Puedes ejecutarlo desde el menú de aplicaciones o con el comando 'DescargaloTo'"
echo "Si tienes problemas con librerías, usa 'descargaloto-safe'"

exit 0
EOF
chmod +x "$PACKAGE_DIR/DEBIAN/postinst"

# prerm
cat > "$PACKAGE_DIR/DEBIAN/prerm" << 'EOF'
#!/bin/bash
# Script pre-eliminación para DescargaloTó

set -e

# Limpiar cache de iconos
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor 2>/dev/null || true
fi

# Actualizar base de datos de aplicaciones
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database /usr/local/share/applications 2>/dev/null || true
fi

echo "Preparando para desinstalar DescargaloTó..."

exit 0
EOF
chmod +x "$PACKAGE_DIR/DEBIAN/prerm"

# postrm
cat > "$PACKAGE_DIR/DEBIAN/postrm" << 'EOF'
#!/bin/bash
# Script post-eliminación para DescargaloTó

set -e

# Limpiar cache de iconos después de desinstalar
if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor 2>/dev/null || true
fi

# Actualizar base de datos de aplicaciones después de desinstalar
if command -v update-desktop-database >/dev/null 2>&1; then
    update-desktop-database /usr/local/share/applications 2>/dev/null || true
fi

echo "DescargaloTó ha sido desinstalado correctamente."

exit 0
EOF
chmod +x "$PACKAGE_DIR/DEBIAN/postrm"

# Paso 9: Calcular tamaño instalado
print_step "9️⃣  Calculando tamaño del paquete..."
INSTALLED_SIZE=$(du -sk "$PACKAGE_DIR" | cut -f1)
sed -i "s/Installed-Size: 2048/Installed-Size: $INSTALLED_SIZE/" "$PACKAGE_DIR/DEBIAN/control"

# Paso 10: Crear el paquete .deb
print_step "🔟 Creando paquete .deb..."

# Limpiar paquete anterior si existe
if [ -f "$DEB_FILE" ]; then
    rm "$DEB_FILE"
fi

# Crear el paquete
if ! dpkg-deb --build "$PACKAGE_DIR" "$DEB_FILE"; then
    print_error "❌ Error al crear el paquete .deb"
    exit 1
fi

# Verificar el paquete
print_step "✅ Verificando paquete..."
if dpkg-deb --info "$DEB_FILE" > /dev/null 2>&1; then
    print_status "✅ Paquete creado exitosamente: $DEB_FILE"
else
    print_error "❌ El paquete creado no es válido"
    exit 1
fi

# Mostrar información del paquete
print_step "📋 Información del paquete:"
echo
dpkg-deb --info "$DEB_FILE"
echo
dpkg-deb --contents "$DEB_FILE"
echo

print_status "🎉 ¡Paquete .deb creado exitosamente!"
print_status "📦 Archivo: $DEB_FILE"
print_status "📏 Tamaño: $(ls -lh "$DEB_FILE" | awk '{print $5}')"
echo
print_status "🚀 Para instalar el paquete:"
echo "  sudo dpkg -i $DEB_FILE"
echo "  sudo apt-get install -f  # Si hay problemas con dependencias"
echo
print_status "🗑️  Para desinstalar:"
echo "  sudo apt-get remove descargaloto"
echo
print_status "🔍 Para probar el paquete:"
echo "  dpkg-deb --contents $DEB_FILE  # Ver contenidos"
echo "  dpkg-deb --info $DEB_FILE      # Ver información"

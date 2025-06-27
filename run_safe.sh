#!/bin/bash

# Script de diagnóstico y ejecución para DescargaloTó
# Soluciona problemas comunes con Qt y librerías

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
    print_error "El ejecutable no existe. Ejecuta 'make build' primero."
    exit 1
fi

print_status "Iniciando DescargaloTó..."

# Verificar librerías Qt
print_status "Verificando dependencias de Qt..."

# Intentar detectar la instalación de Qt
QT_PATHS=(
    "/usr/lib/x86_64-linux-gnu/qt6"
    "/usr/lib/qt6"
    "/usr/local/lib/qt6"
    "/opt/qt6"
)

QT_FOUND=false
for path in "${QT_PATHS[@]}"; do
    if [ -d "$path" ]; then
        print_status "Qt6 encontrado en: $path"
        export QT_PLUGIN_PATH="$path/plugins"
        QT_FOUND=true
        break
    fi
done

if [ "$QT_FOUND" = false ]; then
    print_warning "No se encontró una instalación estándar de Qt6"
fi

# Limpiar variables de entorno problemáticas
unset LD_LIBRARY_PATH
unset LD_PRELOAD

# Variables específicas para evitar conflictos con Snap
export QT_QPA_PLATFORM_PLUGIN_PATH=""
export SNAP=""
export SNAP_NAME=""
export SNAP_VERSION=""

# Configuración de Qt
export QT_AUTO_SCREEN_SCALE_FACTOR=0
export QT_SCALE_FACTOR=1
export QT_LOGGING_RULES="*.debug=false"

# Intentar ejecutar con diferentes métodos
print_status "Intentando ejecutar el programa..."

# Método 1: Ejecución directa
print_status "Método 1: Ejecución directa..."
cd build
if timeout 5s ./DescargaloTo --version 2>/dev/null; then
    print_status "✅ El programa puede ejecutarse directamente"
    exec ./DescargaloTo "$@"
elif [ $? -eq 124 ]; then
    print_status "✅ El programa se está ejecutando (timeout esperado para GUI)"
    exec ./DescargaloTo "$@"
fi

# Método 2: Con LD_LIBRARY_PATH explícito
print_status "Método 2: Con librerías del sistema..."
if command -v ldconfig >/dev/null 2>&1; then
    SYSTEM_LIB_PATH=$(ldconfig -p | grep libQt6Core | head -1 | awk '{print $NF}' | xargs dirname)
    if [ -n "$SYSTEM_LIB_PATH" ]; then
        export LD_LIBRARY_PATH="$SYSTEM_LIB_PATH:$LD_LIBRARY_PATH"
        print_status "Usando librerías de: $SYSTEM_LIB_PATH"
    fi
fi

if timeout 5s ./DescargaloTo --version 2>/dev/null; then
    print_status "✅ El programa funciona con librerías del sistema"
    exec ./DescargaloTo "$@"
elif [ $? -eq 124 ]; then
    print_status "✅ El programa se está ejecutando (timeout esperado para GUI)"
    exec ./DescargaloTo "$@"
fi

# Método 3: Verificar y mostrar dependencias
print_warning "Problemas para ejecutar. Mostrando información de diagnóstico..."
echo ""
print_status "Dependencias del ejecutable:"
ldd ./DescargaloTo | head -20

echo ""
print_status "Variables de entorno relevantes:"
env | grep -E "(QT_|LD_|SNAP)" | sort

echo ""
print_error "No se pudo ejecutar el programa. Posibles soluciones:"
echo "1. Instalar Qt6 del repositorio oficial:"
echo "   sudo apt update && sudo apt install qt6-base-dev qt6-tools-dev"
echo ""
echo "2. Si tienes Qt6 instalado via Snap, desinstálalo:"
echo "   sudo snap remove qt6"
echo ""
echo "3. Recompilar con Qt6 del sistema:"
echo "   make clean && make build"
echo ""
echo "4. Ejecutar manualmente:"
echo "   cd build && LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu ./DescargaloTo"

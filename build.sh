#!/bin/bash

# Script de compilación para DescargaloTó
# Este script compila el proyecto automáticamente

set -e  # Salir si hay algún error

echo "🚀 Compilando DescargaloTó..."

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Función para imprimir mensajes con color
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Verificar dependencias
print_status "Verificando dependencias..."

if ! command -v cmake &> /dev/null; then
    print_error "CMake no está instalado"
    echo "En Ubuntu/Debian: sudo apt install cmake"
    exit 1
fi

if ! command -v qmake6 &> /dev/null && ! command -v qmake &> /dev/null; then
    print_error "Qt6 no está instalado o no está en el PATH"
    echo "En Ubuntu/Debian: sudo apt install qt6-base-dev qt6-tools-dev"
    exit 1
fi

# Generar icono si no existe
if [ ! -f "img/logo.png" ]; then
    print_warning "No se encontró img/logo.png"
    if command -v convert &> /dev/null; then
        print_status "Generando icono con ImageMagick..."
        chmod +x generate_icon.sh
        ./generate_icon.sh
    else
        print_warning "ImageMagick no está instalado, creando icono básico..."
        mkdir -p img
        # Crear un icono básico de 64x64 píxeles (en realidad solo un archivo placeholder)
        echo "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNkYPhfDwAChwGA60e6kgAAAABJRU5ErkJggg==" | base64 -d > img/logo.png
    fi
fi

# Crear directorio de build
print_status "Preparando directorio de build..."
mkdir -p build
cd build

# Configurar con CMake
print_status "Configurando proyecto con CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compilar
print_status "Compilando..."
CPU_COUNT=$(nproc 2>/dev/null || echo 4)
make -j$CPU_COUNT

# Verificar que se compiló correctamente
if [ -f "DescargaloTo" ]; then
    print_status "✅ Compilación exitosa!"
    print_status "Ejecutable creado: build/DescargaloTo"
    
    # Mostrar información del ejecutable
    ls -lh DescargaloTo
    
    echo ""
    print_status "Para ejecutar el programa:"
    echo "  cd build && ./DescargaloTo"
    echo ""
    print_status "Para instalar el programa:"
    echo "  sudo make install"
    
else
    print_error "❌ Error en la compilación"
    exit 1
fi

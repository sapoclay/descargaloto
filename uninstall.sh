#!/bin/bash

# Script de desinstalación para DescargaloTó

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

# Verificar permisos de administrador
if [ "$EUID" -ne 0 ]; then
    print_warning "Se necesitan permisos de administrador para desinstalar."
    print_status "Ejecutando con sudo..."
    exec sudo "$0" "$@"
fi

print_status "Desinstalando DescargaloTó..."

# Eliminar ejecutable
if [ -f "/usr/local/bin/DescargaloTo" ]; then
    print_status "Eliminando ejecutable..."
    rm /usr/local/bin/DescargaloTo
fi

# Eliminar script wrapper
if [ -f "/usr/local/bin/descargaloto-safe" ]; then
    print_status "Eliminando script de ejecución segura..."
    rm /usr/local/bin/descargaloto-safe
fi

# Eliminar iconos
if [ -f "/usr/local/share/icons/hicolor/128x128/apps/descargaloto.png" ]; then
    print_status "Eliminando iconos..."
    rm /usr/local/share/icons/hicolor/128x128/apps/descargaloto.png
fi

if [ -f "/usr/local/share/pixmaps/descargaloto.png" ]; then
    rm /usr/local/share/pixmaps/descargaloto.png
fi

# Eliminar archivo .desktop
if [ -f "/usr/local/share/applications/descargaloto.desktop" ]; then
    print_status "Eliminando entrada del menú..."
    rm /usr/local/share/applications/descargaloto.desktop
fi

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

print_status "✅ DescargaloTó desinstalado correctamente!"

# Función para limpiar configuración del usuario
cleanup_user_config() {
    local username="$1"
    local user_home="$2"
    
    print_status "Limpiando configuración del usuario: $username"
    
    # Eliminar configuración por defecto
    local default_config="$user_home/.config/SapoClay/DescargaloTó.conf"
    if [ -f "$default_config" ]; then
        print_status "  - Eliminando configuración por defecto: $default_config"
        rm "$default_config"
        
        # Eliminar directorio si está vacío
        rmdir "$user_home/.config/SapoClay" 2>/dev/null || true
    fi
    
    # Buscar y eliminar configuraciones personalizadas
    if [ -f "$default_config" ]; then
        # Leer directorio personalizado desde la configuración
        local custom_dir=$(grep "customConfigDir=" "$default_config" 2>/dev/null | cut -d'=' -f2 | tr -d '"')
        if [ -n "$custom_dir" ] && [ -d "$custom_dir" ]; then
            local custom_config="$custom_dir/DescargaloTo_config.ini"
            if [ -f "$custom_config" ]; then
                print_status "  - Eliminando configuración personalizada: $custom_config"
                rm "$custom_config"
                
                # Preguntar si eliminar el directorio personalizado
                echo -n "  ¿Eliminar directorio personalizado $custom_dir? [y/N]: "
                read -r response
                if [[ "$response" =~ ^[Yy]$ ]]; then
                    rmdir "$custom_dir" 2>/dev/null || print_warning "  No se pudo eliminar $custom_dir (no está vacío)"
                fi
            fi
        fi
    fi
}

# Preguntar si eliminar configuración de usuario
echo
echo -e "${YELLOW}¿Deseas eliminar también la configuración y el historial de descargas?${NC}"
echo "Esto incluye:"
echo "  - Configuración por defecto en ~/.config/SapoClay/"
echo "  - Configuración personalizada (si existe)"
echo "  - Historial de descargas"
echo
echo -n "Eliminar configuración [y/N]: "
read -r cleanup_response

if [[ "$cleanup_response" =~ ^[Yy]$ ]]; then
    print_status "Limpiando configuración de usuarios..."
    
    # Obtener información del usuario que ejecutó sudo (si aplica)
    if [ -n "$SUDO_USER" ]; then
        user_home=$(eval echo ~$SUDO_USER)
        cleanup_user_config "$SUDO_USER" "$user_home"
    else
        # Si no hay SUDO_USER, limpiar configuración de root
        cleanup_user_config "root" "/root"
    fi
    
    print_status "✅ Configuración eliminada!"
else
    print_warning "Configuración del usuario conservada en ~/.config/"
fi

#!/bin/bash

# Script de ejecución ultra-limpio para DescargaloTó
# Elimina completamente cualquier interferencia de Snap

# Verificar que el ejecutable existe
if [ ! -f "build/DescargaloTo" ]; then
    echo "Error: El ejecutable no existe. Ejecuta 'make build' primero."
    exit 1
fi

echo "🚀 Iniciando DescargaloTó con entorno ultra-limpio..."

# Crear un entorno completamente nuevo sin variables de Snap
exec env -i \
    HOME="$HOME" \
    USER="$USER" \
    PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
    DISPLAY="$DISPLAY" \
    XAUTHORITY="$XAUTHORITY" \
    XDG_RUNTIME_DIR="$XDG_RUNTIME_DIR" \
    XDG_SESSION_TYPE="x11" \
    QT_QPA_PLATFORM="xcb" \
    QT_QPA_PLATFORM_PLUGIN_PATH="" \
    bash -c 'cd build && ./DescargaloTo "$@"' -- "$@"

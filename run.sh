#!/bin/bash

# Script de ejecución para DescargaloTó
# Soluciona problemas con librerías de Snap

# Verificar que el ejecutable existe
if [ ! -f "build/DescargaloTo" ]; then
    echo "Error: El ejecutable no existe. Ejecuta 'make build' primero."
    exit 1
fi

# Cambiar al directorio build y ejecutar el programa con entorno limpio
cd build
exec env -u LD_LIBRARY_PATH -u LD_PRELOAD QT_QPA_PLATFORM_PLUGIN_PATH="" ./DescargaloTo "$@"

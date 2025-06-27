# 📥 DescargaloTó - Gestor de descargas

![DescargaloTó](img/logo.png)

**DescargaloTó** es un gestor de descargas que me acabo de hacer, por que puedo y por que lo necesito. Está desarrollado en C++ con Qt6, lo cual permite descargar archivos desde URLs de forma sencilla y eficiente con una interfaz gráfica muy facilita y sencilla de usar. Guarda registro de las URL's descargadas para volver a utilizar las mismas URL's en otro momento sin tener que volver a buscarlas ni nada de eso... Vamos, es algo que me resulta útil en momentos puntuales.

## ✨ Características principales

- 🖥️ **Interfaz gráfica intuitiva**
- 📥 **Gestión de múltiples descargas** desde URLs
- 📜 **Historial completo de descargas** con fechas
- 🎯 **Selección inteligente**: El botón "Descargar Ahora" se activa seleccionando la URL guardada en el historial
- 🎨 **Temas seleccionables**: Entre un tema claro y otro oscuro
- � **Configuración personalizada**: El programa permite elegir dónde guardar el historial de descargas, para tenerlo a mano.
- 🔄 **Migración automática**: Mueve el historial al cambiar de directorio
- �🔔 **Integración con bandeja del sistema**
- � **Configuración de carpeta de destino**
- 🗑️ **Desinstalación limpia**: Elimina configuración y historial al desinstalar
- 🛡️ **Manejo de errores**
- 🐧 **Multiplataforma**: Por el momento funciona en Linux y Windows

## 🚀 Instalación Rápida

### Ubuntu/Debian (.deb)
Para utilizar el paquete .deb, solo hay que dirigirse a la paǵina de lanzamientos y [descargar desde ahí la última versión publicado del paquete](https://github.com/sapoclay/descargaloto/releases/) Una vez que tengamos guardado el paquete basta con ejecutar en una terminal (Ctrl+Alt+T) el comando:
```bash
# Descargar e instalar el paquete .deb
sudo apt install ./descargaloto_1.0.3_amd64.deb
```
Tras la instalación, se puede buscar el lanzador en nuestro equipo o siemplemente ejecutar en la terminal:

```bash
# Ejecutar
DescargaloTo
# o modo seguro si hay problemas:
descargaloto-safe
```

### Desde código fuente
```bash
# Clonar repositorio
git clone https://github.com/sapoclay/descargaloto.git
cd descargaloto

# Compilar
make clean
make build

# Ejecutar
make run

# Instalar en el sistema (opcional)
sudo make install
```

## 🎮 Cómo usar

### 1. Añadir URLs al Historial
1. Introduce una URL en el campo "Nueva Descarga"
2. Haz clic en **"Añadir al Historial"** o presiona Enter
3. La URL se guardará en el historial con fecha y hora

### 2. Descargar archivos
1. **Selecciona una URL** del historial (clic en una fila)
2. Haz clic en **"Descargar Ahora"** (se activa solo con selección)
3. Elige la carpeta de destino
4. Especifica el nombre del archivo que quieres guardar (añadiendo la extensión si es necesaria)
5. ¡La descarga comenzará automáticamente!

### 3. Gestionar historial
- **Eliminar Seleccionada**: Borra la URL seleccionada
- **Limpiar Historial**: Elimina todas las URLs guardadas

### 4. Personalización
- Ve a **Opciones → Preferencias**
- Elige entre **Tema Claro** y **Tema Oscuro**
- Los cambios se aplican inmediatamente

### 🚀 Métodos de ejecución

#### ✅ **Método recomendado (Desarrollo)**
```bash
make run                   # Entorno ultra-limpio automático
```

#### ✅ **Después de instalar el paquete**
```bash
DescargaloTo              # Comando principal con detección automática
descargaloto-safe         # Entorno ultra-limpio garantizado
```

#### ✅ **Scripts alternativos**
```bash
./run_clean.sh           # Script manual ultra-limpio
make run-safe            # Para diagnóstico si hay problemas
```

#### ❌ **NUNCA uses los siguientes comandos (causarán errores con Snap):**
```bash
# ❌ NO HAGAS ESTO:
cd build && ./DescargaloTo
./build/DescargaloTo
```
## 🎨 Interfaz de usuario

### Secciones principales

1. **Nueva Descarga** 
   - Campo de entrada para URLs
   - Botón "Añadir al Historial"

2. **Estado de Descarga**
   - Barra de progreso en tiempo real
   - Log detallado de operaciones
   - Estado actual de la descarga

3. **Historial de Descargas**
   - Lista de URLs con fechas
   - **Descargar Ahora** (activado por selección)
   - **Eliminar Seleccionada** / **Limpiar Historial**

### Flujo de Trabajo Optimizado
```
Introducir URL → Añadir al Historial → Seleccionar del Historial → Descargar Ahora
```

## � Configuración personalizada del historial

### 🔧 Directorio de configuración personalizado

DescargaloTó permite elegir dónde guardar el historial de descargas y la configuración.

#### **Configuración**:
1. **Abrir Preferencias**: `Opciones` → `Preferencias...`
2. **Configurar Directorio**: En "Directorio de configuración"
   - Examinar y seleccionar la carpeta deseada
   - Dejar vacío para usar el directorio por defecto

#### **Ubicaciones**:

**Directorio Por Defecto** (Linux):
```bash
~/.config/nombre-usuario/DescargaloTó.conf
```

**Directorio Personalizado**:
```bash
[directorio_elegido]/DescargaloTo_config.ini
```

#### **🔄 Migración Automática**:
- El historial actual se **migra automáticamente** al nuevo directorio
- Se mantienen todas las URLs y configuraciones
- El archivo anterior se conserva hasta confirmar el cambio

#### **📋 Contenido del Archivo**:
```ini
[General]
downloadHistory=url1, url2, url3, ...
theme=0
customConfigDir=/ruta/al/directorio/personalizado
```

#### **🗑️ Desinstalación**:

**Script Manual** (`./uninstall.sh`):
- Pregunta si eliminar la configuración
- Limpia tanto directorio por defecto como personalizado

**Paquete .deb**:
- **Normal**: `sudo apt remove descargaloto` - Conserva configuración
- **Completa**: `sudo apt purge descargaloto` - Elimina toda la configuración

#### **🔍 Comandos Útiles**:
```bash
# Ver configuración actual
cat ~/.config/SapoClay/DescargaloTó.conf

# Limpiar configuración manualmente
rm ~/.config/SapoClay/DescargaloTó.conf
```

## �🔧 Requisitos del sistema

### Linux (Ubuntu/Debian)
- Ubuntu 22.04 LTS o superior
- Debian 11 (Bullseye) o superior
- Qt6 (Core, Widgets, Network)
- Dependencias se instalan automáticamente con el .deb

### Compilación desde fuente
- **Qt6**: libqt6-dev, qt6-base-dev
- **CMake**: 3.16 o superior
- **Compilador**: GCC con soporte C++17
- **Build tools**: make, build-essential

```bash
# Instalar dependencias en Ubuntu/Debian:
sudo apt update
sudo apt install qt6-base-dev libqt6-dev cmake build-essential
```

## 🛠️ Desarrollo

### Estructura del Proyecto
```
descargaloto/
├── src/                     # Código fuente C++
│   ├── main.cpp            # Punto de entrada
│   ├── mainwindow.*        # Ventana principal
│   ├── downloadmanager.*   # Gestor de descargas
│   ├── preferencesdialog.* # Diálogo de preferencias
│   ├── aboutdialog.*       # Diálogo "Acerca de"
│   └── systemtrayicon.*    # Icono de bandeja
├── img/                    # Recursos gráficos
├── build/                  # Archivos compilados
├── package/                # Estructura del paquete .deb
├── CMakeLists.txt          # Configuración CMake
├── Makefile               # Comandos de desarrollo
└── *.md                   # Documentación
```

### Comandos de desarrollo
```bash
make clean          # Limpiar archivos de build
make build          # Compilar proyecto
make run            # Ejecutar con entorno limpio (recomendado)
make run-safe       # Ejecutar con diagnóstico
make install        # Instalar en el sistema
make deb            # Crear paquete .deb
make icon           # Generar icono
make deps           # Verificar dependencias
make help           # Mostrar ayuda completa
```

## 🚀 Guía detallada de ejecución

### ⚠️ Importante: Problemas Conocidos con Snap

Si ejecutas directamente el binario puede aparecer este error:
```
symbol lookup error: /snap/core20/current/lib/x86_64-linux-gnu/libpthread.so.0: undefined symbol: __libc_pthread_init
```

### ✅ Métodos de ejecución seguros

#### **1. Durante desarrollo (Recomendado)**
```bash
make run                    # ✅ Entorno ultra-limpio automático
```

#### **2. Después de instalar el paquete**
```bash
DescargaloTo               # ✅ Detección automática de entorno
descargaloto-safe          # ✅ Entorno ultra-limpio garantizado (con mensaje)
```

#### **3. Scripts alternativos de desarrollo**
```bash
./run_clean.sh            # ✅ Script manual ultra-limpio
make run-safe             # ✅ Para diagnóstico si hay problemas
```

#### **4. Ejecución manual avanzada**
```bash
cd build
env -i \
    HOME="$HOME" \
    USER="$USER" \
    PATH="/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
    DISPLAY="${DISPLAY:-:0}" \
    XAUTHORITY="${XAUTHORITY}" \
    XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR}" \
    XDG_SESSION_TYPE="x11" \
    QT_QPA_PLATFORM="xcb" \
    ./DescargaloTo
```

### ❌ **NUNCA uses estos comandos** (causarán errores):
```bash
# ❌ NO HAGAS ESTO - Fallará con Snap:
cd build && ./DescargaloTo
./build/DescargaloTo
build/DescargaloTo
```

### 🔧 ¿Qué hace el entorno ultra-limpio?

El entorno limpio:
- ✅ Elimina **todas** las variables de Snap
- ✅ Conserva solo variables esenciales para Qt/X11  
- ✅ Usa un PATH limpio del sistema
- ✅ Garantiza que no hay interferencias
- ✅ Funciona en el 100% de los casos en los que lo he probado

### 📦 Instalación con paquete .deb

#### **Crear e Instalar el Paquete**:
```bash
# Crear el paquete
make deb

# Instalar
sudo apt install ./descargaloto_1.0.3_amd64.deb
```

#### **Ventajas del paquete .deb**:
- ✅ **Integración completa** - Aparece en el menú de aplicaciones
- ✅ **Gestión de dependencias** - APT instala todo automáticamente
- ✅ **Desinstalación limpia** - `sudo apt remove descargaloto`
- ✅ **Scripts optimizados** - Incluye todas las soluciones
- ✅ **Iconos y accesos directos** - Todo configurado automáticamente

#### **Después de instalar el .deb**:
```bash
# Ejecutar desde el menú de aplicaciones o:
DescargaloTo              # Comando principal
descargaloto-safe         # Si hay cualquier problema
```

### 🎯 Resultado Esperado

Cuando ejecutes correctamente, verás:
```bash
▶️  Ejecutando DescargaloTó...
🚀 Iniciando DescargaloTó con entorno ultra-limpio...
[El programa se abre correctamente en la bandeja del sistema]
```

### Problemas de dependencias
```bash
# Verificar Qt6
apt list --installed | grep qt6

# Reinstalar si es necesario
sudo apt update
sudo apt install --fix-broken
```
## 🎯 Versiones

### v1.0.0 (Inicial)
- ✅ Funcionalidad básica de descarga
- ✅ Historial de URLs
- ✅ Interfaz gráfica completa
- ✅ Integración con bandeja del sistema

### v1.0.1 (Mejoras de UI)
- ✅ **Temas claro y oscuro personalizables**
- ✅ **Contraste mejorado** en tema claro
- ✅ **Cambio múltiple de temas** corregido
- ✅ **Botón "Descargar Ahora" movido al historial**
- ✅ **Activación inteligente** por selección

### v1.0.2 (Configuración Personalizada + Solución Definitiva Snap)
- ✅ **Directorio personalizable** para guardar el historial
- ✅ **Migración automática** del historial al cambiar directorio
- ✅ **Desinstalación limpia** con eliminación de configuración
- ✅ **Interfaz mejorada** en preferencias con opciones avanzadas
- ✅ **Solución definitiva para problemas de Snap/librerías**
- ✅ **Detección automática de entorno** (Snap vs sistema limpio)
- ✅ **Comando DescargaloTo inteligente** que se adapta al entorno
- ✅ **Script descargaloto-safe** con entorno ultra-limpio garantizado

### v1.0.3 (Mejoras de Temas y UI)
- ✅ **AboutDialog con soporte de temas** - Cambia correctamente entre claro/oscuro
- ✅ **Botón "Ver en GitHub" con icono real** - SVG vectorial que se ve en ambos temas
- ✅ **Fuente blanca consistente en modo oscuro** - Toda la aplicación usa texto blanco
- ✅ **Tema dinámico en AboutDialog** - Se aplica automáticamente el tema actual
- ✅ **Mejor contraste y legibilidad** en ambos temas

## 📄 Licencia

Este proyecto está licenciado bajo los términos especificados en el archivo `LICENSE`.

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Para cambios importantes:

1. Abre un issue para discutir el cambio
2. Fork el proyecto
3. Crea una feature branch
4. Commit tus cambios
5. Push a la branch
6. Abre un Pull Request

## 🎉 ¡Empezar!

```bash
# Instalación más rápida:
sudo apt install ./descargaloto_1.0.3_amd64.deb

# Primera ejecución:
DescargaloTo

# ¡Descargonizamiento ....! 🚀
```

---

**DescargaloTó** - *Descargas simples, gestión inteligente del tiempo* 📥✨

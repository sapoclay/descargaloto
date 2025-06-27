cmake_minimum_required(VERSION 3.16)
project(DescargaloTo VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Habilitar el procesamiento automático de MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Definir el ejecutable
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/downloadmanager.cpp
    src/aboutdialog.cpp
    src/systemtrayicon.cpp
    src/preferencesdialog.cpp
)

set(HEADERS
    src/mainwindow.h
    src/downloadmanager.h
    src/aboutdialog.h
    src/systemtrayicon.h
    src/preferencesdialog.h
)

# Crear el archivo de recursos
set(RESOURCES
    resources.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# Enlazar con Qt
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets Qt6::Network)

# Configurar el icono para Windows
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Instalar el ejecutable
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Parcial No. 2 - Administración de Memoria

Sistemas Operativos - Universidad del Valle

Dos simuladores en C++ (paradigma orientado a objetos):

1. **`memoria/`** — Asignación de memoria contigua con particiones dinámicas:
First Fit, Best Fit y Worst Fit.
2. **`paginacion/`** — Traducción de direcciones virtuales a físicas mediante
una tabla de páginas de un nivel.

Ambos se pueden ejecutar de forma nativa (con `g++`) o mediante Docker.
Estas instrucciones están pensadas para **WSL (Ubuntu)**.

\---

## 1\. Requisitos en WSL

```bash
sudo apt update
sudo apt install -y g++ make
# Docker Desktop con integración WSL2, o docker-ce dentro de WSL
docker --version
docker compose version
```

\---

## 2\. Compilar y ejecutar de forma nativa

### Simulador de memoria

```bash
cd memoria
g++ -std=c++17 -Wall -O2 -o memoria src/main.cpp src/AdministradorMemoria.cpp

# Menú interactivo:
./memoria

# Ejecutar directamente un archivo de entrada (además abre el menú):
./memoria entradas/entrada1.txt
```

Formato del archivo de entrada (`memoria/entradas/\\\\\\\*.txt`):

```
INICIAR <tamanoTotal> <FF|BF|WF>
ALLOC <pid> <tamano>
FREE <pid>
ESTRATEGIA <FF|BF|WF>
ESTADO
```

### Simulador de paginación

```bash
cd paginacion
g++ -std=c++17 -Wall -O2 -o paginacion src/main.cpp src/SimuladorPaginacion.cpp

./paginacion
./paginacion entradas/entrada1.txt
```

Formato del archivo de entrada (`paginacion/entradas/\\\\\\\*.txt`):

```
INICIAR <tamMemoriaVirtual> <tamMemoriaFisica> <tamPagina>
TRADUCIR <direccionVirtual>
LIBERAR <numeroDePagina>
TABLA
CONFIG
```

\---

## 3\. Ejecutar con Docker (recomendado para la entrega)

Desde la raíz del proyecto (`so-parcial2/`):

```bash
# Construir ambas imágenes
docker compose build

# Ejecutar el simulador de memoria de forma interactiva
docker compose run --rm memoria

# Ejecutar el simulador de paginación de forma interactiva
docker compose run --rm paginacion

# También puedes pasarle directamente un archivo de entrada:
docker compose run --rm memoria ./memoria entradas/entrada1.txt
docker compose run --rm paginacion ./paginacion entradas/entrada1.txt
```

Los archivos de salida generados quedan disponibles en tu máquina gracias
al volumen montado (`memoria/salidas/` y `paginacion/salidas/`), lo cual
sirve como evidencia del despliegue en Docker para el informe (captura de
pantalla de `docker compose build` + `docker compose run` + contenido de
`salidas/`).

Alternativa sin `docker compose`:

```bash
cd memoria
docker build -t so-p2-memoria .
docker run -it --rm -v $(pwd)/salidas:/app/salidas so-p2-memoria
```

\---

## 4\. Estructura del proyecto

```
so-parcial2/
├── docker-compose.yml
├── README.md
├── memoria/
│   ├── Dockerfile
│   ├── entradas/          # archivos de entrada de ejemplo
│   ├── salidas/           # se genera al ejecutar (archivos de salida)
│   └── src/
│       ├── Bloque.h
│       ├── Proceso.h
│       ├── AdministradorMemoria.h/.cpp
│       └── main.cpp
└── paginacion/
    ├── Dockerfile
    ├── entradas/
    ├── salidas/
    └── src/
        ├── TablaPaginas.h
        ├── SimuladorPaginacion.h/.cpp
        └── main.cpp
```

## 5\. Diseño orientado a objetos (resumen)

**Simulador de memoria**

* `Bloque`: entidad que representa un segmento de memoria (libre u ocupado).
* `Proceso`: solicitud de memoria de un proceso.
* `AdministradorMemoria`: mantiene la lista de bloques y encapsula la
lógica de las tres estrategias de búsqueda (First/Best/Worst Fit),
división de bloques, liberación y fusión de bloques libres contiguos
(coalescing), y el cálculo de fragmentación externa.

**Simulador de paginación**

* `EntradaPagina` / `TablaPaginas`: modelan la tabla de páginas de un nivel
(arreglo de entradas página→marco).
* `SimuladorPaginacion`: actúa como una MMU simplificada. Calcula número de
página y desplazamiento a partir de la dirección virtual, gestiona los
marcos libres con una cola (FIFO de asignación, no de reemplazo), resuelve
fallos de página asignando el siguiente marco disponible y traduce a
dirección física.

## 6\. Nota sobre el uso de IA

Este proyecto fue desarrollado con apoyo de Claude (Anthropic) como
asistente de programación. Ver la sección correspondiente en el informe
para el detalle del prompt utilizado y la personalización realizada.




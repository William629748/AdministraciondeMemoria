# Imagen para el Simulador de Asignacion de Memoria (First/Best/Worst Fit)
FROM gcc:13-bookworm

WORKDIR /app

COPY src/ ./src/
COPY entradas/ ./entradas/
RUN mkdir -p salidas

RUN g++ -std=c++17 -Wall -O2 -o memoria src/main.cpp src/AdministradorMemoria.cpp

# Se ejecuta en modo interactivo: docker run -it <imagen>
ENTRYPOINT ["./memoria"]

# Imagen para el Simulador de Paginacion de un nivel (traduccion de
# direcciones virtuales a fisicas)
FROM gcc:13-bookworm

WORKDIR /app

COPY src/ ./src/
COPY entradas/ ./entradas/
RUN mkdir -p salidas

RUN g++ -std=c++17 -Wall -O2 -o paginacion src/main.cpp src/SimuladorPaginacion.cpp

# Se ejecuta en modo interactivo: docker run -it <imagen>
ENTRYPOINT ["./paginacion"]

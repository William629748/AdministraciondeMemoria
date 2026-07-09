#ifndef PROCESO_H
#define PROCESO_H

#include <string>

// Representa la solicitud de un proceso que necesita memoria.
class Proceso {
public:
    std::string id;
    int tamano; // memoria solicitada

    Proceso(std::string id_, int tamano_) : id(id_), tamano(tamano_) {}
};

#endif


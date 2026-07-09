#ifndef BLOQUE_H
#define BLOQUE_H

#include <string>

// Representa un bloque contiguo de memoria dentro del arreglo/lista que
// modela la memoria principal. Cada bloque puede estar libre u ocupado
// por un proceso.
class Bloque {
public:
    int inicio;      // Dirección donde inicia el bloque
    int tamano;       // Tamaño del bloque en unidades de memoria (por ejemplo KB)
    bool libre;        // true = bloque libre, false = ocupado
    std::string pid;   // Identificador del proceso que lo ocupa ("" si libre)

    Bloque(int inicio_, int tamano_, bool libre_ = true, std::string pid_ = "")
        : inicio(inicio_), tamano(tamano_), libre(libre_), pid(pid_) {}
};

#endif


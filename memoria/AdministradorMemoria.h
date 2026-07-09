#ifndef ADMINISTRADOR_MEMORIA_H
#define ADMINISTRADOR_MEMORIA_H

#include <vector>
#include <string>
#include "Bloque.h"

// Estrategias de asignación de memoria contigua (particiones dinámicas),
//  First Fit, Best Fit y Worst Fit.
enum class Estrategia { FIRST_FIT, BEST_FIT, WORST_FIT };

// AdministradorMemoria modela la memoria principal como una lista de
// bloques contiguos (libres u ocupados).
class AdministradorMemoria {
private:
    int tamanoTotal;
    Estrategia estrategia;
    std::vector<Bloque> bloques;

    // Busca el índice del bloque libre según la estrategia activa.
    // Retorna -1 si no hay bloque que satisfaga la solicitud.
    int buscarBloque(int tamanoSolicitado);

    // Fusiona bloques libres adyacentes tras una liberación, para reducir
    // la fragmentación externa (coalescing).
    void fusionarLibres();

public:
    AdministradorMemoria(int tamanoTotal_, Estrategia estrategia_);

    void setEstrategia(Estrategia e);
    Estrategia getEstrategia() const;
    static std::string nombreEstrategia(Estrategia e);

    // Intenta asignar memoria a un proceso. Retorna true si fue posible.
    // fragInterna, si aplica (partición se divide y sobra un residuo menor
    // al mínimo configurable), se reporta mediante el parámetro opcional.
    bool asignar(const std::string& pid, int tamanoSolicitado);

    // Libera la memoria ocupada por un proceso.
    bool liberar(const std::string& pid);

    // Reinicia la memoria a un único bloque libre del tamaño total.
    void reiniciar(int nuevoTamano);

    // ---- Reportes ----
    void imprimirEstado(std::ostream& out) const;
    int fragmentacionExterna() const; // suma de espacio libre que quedo repartido en bloques pequeños
    int bloquesLibres() const;
    int bloquesOcupados() const;
    int memoriaLibreTotal() const;
    int memoriaOcupadaTotal() const;

    const std::vector<Bloque>& getBloques() const { return bloques; }
};

#endif


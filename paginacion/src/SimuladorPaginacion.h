#ifndef SIMULADOR_PAGINACION_H
#define SIMULADOR_PAGINACION_H

#include <vector>
#include <queue>
#include <string>
#include <ostream>
#include "TablaPaginas.h"

// Resultado de una traduccion de direccion virtual a fisica.
struct ResultadoTraduccion {
    bool exito;
    int direccionVirtual;
    int numPagina;
    int desplazamiento;    // offset dentro de la pagina
    int marco;
    int direccionFisica;
    bool huboFalloDePagina; // true si la pagina no estaba mapeada y se
                              // tuvo que asignar un marco nuevo
    std::string mensaje;
};

// SimuladorPaginacion actua como una MMU simplificada: administra la
// memoria virtual, la memoria fisica (marcos) y la tabla de paginas de un
// nivel
//
// Esquema de asignacion: paginacion "on-demand" sin reemplazo  cuando se
// traduce una direccion virtual cuya pagina aun no esta mapeada, se le
// asigna el siguiente marco libre disponible (simulando el primer acceso
// de un proceso a esa pagina). Si no quedan marcos libres, se reporta un
// error de memoria fisica agotada (no se implementa reemplazo de paginas,
// pues el alcance de esta entrega es solo la traduccion de direcciones).
class SimuladorPaginacion {
private:
    int tamMemoriaVirtual;  // en bytes (o KB, segun se configure)
    int tamMemoriaFisica;
    int tamPagina;
    int numPaginas;
    int numMarcos;

    TablaPaginas tabla;
    std::queue<int> marcosLibres;
    std::vector<bool> marcoOcupado;

public:
    SimuladorPaginacion(int tamMemoriaVirtual_, int tamMemoriaFisica_, int tamPagina_);

    ResultadoTraduccion traducir(int direccionVirtual);

    // Libera explicitamente una pagina (por ejemplo, al terminar un proceso).
    bool liberarPagina(int numPagina);

    void imprimirConfiguracion(std::ostream& out) const;
    void imprimirTablaPaginas(std::ostream& out) const;

    int getNumPaginas() const { return numPaginas; }
    int getNumMarcos() const { return numMarcos; }
    int getTamPagina() const { return tamPagina; }
    int getMarcosLibresDisponibles() const { return static_cast<int>(marcosLibres.size()); }
};

#endif


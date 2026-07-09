#ifndef TABLA_PAGINAS_H
#define TABLA_PAGINAS_H

#include <vector>

// Entrada de la tabla de paginas de un nivel
// Cada entrada asocia una pagina virtual con un marco (frame) de memoria
// fisica, e indica si esa pagina esta actualmente presente (mapeada).
struct EntradaPagina {
    int marco;      // numero de marco fisico asignado (-1 si no mapeada)
    bool presente;  // bit de presencia / validez

    EntradaPagina() : marco(-1), presente(false) {}
};

// Tabla de paginas de un solo nivel: un arreglo indexado por numero de
// pagina virtual.
class TablaPaginas {
private:
    std::vector<EntradaPagina> entradas;

public:
    explicit TablaPaginas(int numPaginas) : entradas(numPaginas) {}

    int numPaginas() const { return static_cast<int>(entradas.size()); }

    bool estaMapeada(int numPagina) const {
        if (numPagina < 0 || numPagina >= numPaginas()) return false;
        return entradas[numPagina].presente;
    }

    int obtenerMarco(int numPagina) const {
        if (numPagina < 0 || numPagina >= numPaginas()) return -1;
        return entradas[numPagina].marco;
    }

    void mapear(int numPagina, int marco) {
        if (numPagina < 0 || numPagina >= numPaginas()) return;
        entradas[numPagina].marco = marco;
        entradas[numPagina].presente = true;
    }

    void desmapear(int numPagina) {
        if (numPagina < 0 || numPagina >= numPaginas()) return;
        entradas[numPagina].marco = -1;
        entradas[numPagina].presente = false;
    }

    const std::vector<EntradaPagina>& getEntradas() const { return entradas; }
};

#endif


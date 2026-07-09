#include "SimuladorPaginacion.h"
#include <iostream>
#include <iomanip>
#include <cmath>

SimuladorPaginacion::SimuladorPaginacion(int tamMemoriaVirtual_, int tamMemoriaFisica_,
                                          int tamPagina_)
    : tamMemoriaVirtual(tamMemoriaVirtual_),
      tamMemoriaFisica(tamMemoriaFisica_),
      tamPagina(tamPagina_),
      numPaginas((tamMemoriaVirtual_ + tamPagina_ - 1) / tamPagina_), // ceil
      numMarcos(tamMemoriaFisica_ / tamPagina_),                      // floor
      tabla(numPaginas),
      marcoOcupado(numMarcos, false) {
    for (int i = 0; i < numMarcos; ++i) marcosLibres.push(i);
}

ResultadoTraduccion SimuladorPaginacion::traducir(int direccionVirtual) {
    ResultadoTraduccion r;
    r.direccionVirtual = direccionVirtual;
    r.exito = false;
    r.huboFalloDePagina = false;

    if (direccionVirtual < 0 || direccionVirtual >= tamMemoriaVirtual) {
        r.mensaje = "Direccion virtual fuera de rango [0, " +
                    std::to_string(tamMemoriaVirtual - 1) + "]";
        return r;
    }

    int pagina = direccionVirtual / tamPagina;
    int offset = direccionVirtual % tamPagina;
    r.numPagina = pagina;
    r.desplazamiento = offset;

    if (!tabla.estaMapeada(pagina)) {
        // Fallo de pagina: se necesita un marco fisico nuevo.
        if (marcosLibres.empty()) {
            r.mensaje = "Fallo de pagina: no hay marcos libres en memoria fisica ";
            return r;
        }
        int marco = marcosLibres.front();
        marcosLibres.pop();
        marcoOcupado[marco] = true;
        tabla.mapear(pagina, marco);
        r.huboFalloDePagina = true;
    }

    int marco = tabla.obtenerMarco(pagina);
    r.marco = marco;
    r.direccionFisica = marco * tamPagina + offset;
    r.exito = true;
    r.mensaje = r.huboFalloDePagina
                    ? "Traduccion exitosa "
                    : "Traduccion exitosa ";
    return r;
}

bool SimuladorPaginacion::liberarPagina(int numPagina) {
    if (!tabla.estaMapeada(numPagina)) return false;
    int marco = tabla.obtenerMarco(numPagina);
    tabla.desmapear(numPagina);
    marcoOcupado[marco] = false;
    marcosLibres.push(marco);
    return true;
}

void SimuladorPaginacion::imprimirConfiguracion(std::ostream& out) const {
    out << "\n=== Configuracion de la MMU (Paginacion de un nivel) ===\n";
    out << "Memoria virtual : " << tamMemoriaVirtual << " (paginas: " << numPaginas << ")\n";
    out << "Memoria fisica  : " << tamMemoriaFisica << " (marcos: " << numMarcos << ")\n";
    out << "Tamano de pagina: " << tamPagina << "\n";
    out << "Marcos libres   : " << marcosLibres.size() << " / " << numMarcos << "\n";
}

void SimuladorPaginacion::imprimirTablaPaginas(std::ostream& out) const {
    out << "\n--- Tabla de paginas ---\n";
    out << std::left << std::setw(10) << "Pagina" << std::setw(10) << "Presente"
        << std::setw(10) << "Marco" << "\n";
    out << std::string(30, '-') << "\n";
    const auto& entradas = tabla.getEntradas();
    for (int i = 0; i < (int)entradas.size(); ++i) {
        out << std::left << std::setw(10) << i
            << std::setw(10) << (entradas[i].presente ? "Si" : "No")
            << std::setw(10) << (entradas[i].presente ? std::to_string(entradas[i].marco) : "-")
            << "\n";
    }
}


#include "AdministradorMemoria.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

AdministradorMemoria::AdministradorMemoria(int tamanoTotal_, Estrategia estrategia_)
    : tamanoTotal(tamanoTotal_), estrategia(estrategia_) {
    bloques.emplace_back(0, tamanoTotal, true, "");
}

void AdministradorMemoria::setEstrategia(Estrategia e) { estrategia = e; }
Estrategia AdministradorMemoria::getEstrategia() const { return estrategia; }

std::string AdministradorMemoria::nombreEstrategia(Estrategia e) {
    switch (e) {
        case Estrategia::FIRST_FIT: return "First Fit";
        case Estrategia::BEST_FIT:  return "Best Fit";
        case Estrategia::WORST_FIT: return "Worst Fit";
    }
    return "Desconocida";
}

void AdministradorMemoria::reiniciar(int nuevoTamano) {
    tamanoTotal = nuevoTamano;
    bloques.clear();
    bloques.emplace_back(0, tamanoTotal, true, "");
}

int AdministradorMemoria::buscarBloque(int tamanoSolicitado) {
    int candidato = -1;

    switch (estrategia) {
        case Estrategia::FIRST_FIT: {
            for (size_t i = 0; i < bloques.size(); ++i) {
                if (bloques[i].libre && bloques[i].tamano >= tamanoSolicitado) {
                    return static_cast<int>(i); // primer bloque que sirve
                }
            }
            break;
        }
        case Estrategia::BEST_FIT: {
            int mejorDif = -1;
            for (size_t i = 0; i < bloques.size(); ++i) {
                if (bloques[i].libre && bloques[i].tamano >= tamanoSolicitado) {
                    int dif = bloques[i].tamano - tamanoSolicitado;
                    if (mejorDif == -1 || dif < mejorDif) {
                        mejorDif = dif;
                        candidato = static_cast<int>(i);
                    }
                }
            }
            break;
        }
        case Estrategia::WORST_FIT: {
            int peorTam = -1;
            for (size_t i = 0; i < bloques.size(); ++i) {
                if (bloques[i].libre && bloques[i].tamano >= tamanoSolicitado) {
                    if (bloques[i].tamano > peorTam) {
                        peorTam = bloques[i].tamano;
                        candidato = static_cast<int>(i);
                    }
                }
            }
            break;
        }
    }
    return candidato;
}

bool AdministradorMemoria::asignar(const std::string& pid, int tamanoSolicitado) {
    if (tamanoSolicitado <= 0) return false;

    int idx = buscarBloque(tamanoSolicitado);
    if (idx == -1) return false; // no hay bloque suficiente -> "sin memoria"

    Bloque& b = bloques[idx];

    if (b.tamano == tamanoSolicitado) {
        // Ajuste exacto: no queda residuo (no hay fragmentación interna
        // porque en particiones dinámicas el bloque se recorta al tamaño
        // exacto solicitado).
        b.libre = false;
        b.pid = pid;
    } else {
        // Se divide el bloque: una parte para el proceso y el resto
        // queda libre.
        int inicioNuevo = b.inicio + tamanoSolicitado;
        int tamanoResiduo = b.tamano - tamanoSolicitado;

        b.tamano = tamanoSolicitado;
        b.libre = false;
        b.pid = pid;

        bloques.insert(bloques.begin() + idx + 1,
                        Bloque(inicioNuevo, tamanoResiduo, true, ""));
    }
    return true;
}

bool AdministradorMemoria::liberar(const std::string& pid) {
    bool encontrado = false;
    for (auto& b : bloques) {
        if (!b.libre && b.pid == pid) {
            b.libre = true;
            b.pid = "";
            encontrado = true;
            break;
        }
    }
    if (encontrado) fusionarLibres();
    return encontrado;
}

void AdministradorMemoria::fusionarLibres() {
    for (size_t i = 0; i + 1 < bloques.size(); ) {
        if (bloques[i].libre && bloques[i + 1].libre) {
            bloques[i].tamano += bloques[i + 1].tamano;
            bloques.erase(bloques.begin() + i + 1);
            // no incrementamos i: puede seguir fusionando con el siguiente
        } else {
            ++i;
        }
    }
}

int AdministradorMemoria::fragmentacionExterna() const {
    // Fragmentación externa: memoria libre que existe pero está repartida
    // en huecos que, individualmente, pueden no ser útiles para procesos
    // futuros. La reportamos como el total de memoria libre cuando hay
    // más de un bloque libre disperso entre ocupados.
    int libres = 0;
    int bloquesLibresCont = 0;
    for (const auto& b : bloques) {
        if (b.libre) {
            libres += b.tamano;
            bloquesLibresCont++;
        }
    }
    // Si toda la memoria libre está en un solo bloque, no hay
    // fragmentación externa "problemática".
    if (bloquesLibresCont <= 1) return 0;
    return libres;
}

int AdministradorMemoria::bloquesLibres() const {
    int c = 0;
    for (const auto& b : bloques) if (b.libre) c++;
    return c;
}

int AdministradorMemoria::bloquesOcupados() const {
    int c = 0;
    for (const auto& b : bloques) if (!b.libre) c++;
    return c;
}

int AdministradorMemoria::memoriaLibreTotal() const {
    int s = 0;
    for (const auto& b : bloques) if (b.libre) s += b.tamano;
    return s;
}

int AdministradorMemoria::memoriaOcupadaTotal() const {
    int s = 0;
    for (const auto& b : bloques) if (!b.libre) s += b.tamano;
    return s;
}

void AdministradorMemoria::imprimirEstado(std::ostream& out) const {
    out << "\n=== Estado de la memoria (" << nombreEstrategia(estrategia)
        << ") | Total: " << tamanoTotal << " ===\n";
    out << std::left
        << std::setw(10) << "Inicio"
        << std::setw(10) << "Tamano"
        << std::setw(10) << "Estado"
        << std::setw(10) << "PID" << "\n";
    out << std::string(38, '-') << "\n";
    for (const auto& b : bloques) {
        out << std::left
            << std::setw(10) << b.inicio
            << std::setw(10) << b.tamano
            << std::setw(10) << (b.libre ? "LIBRE" : "OCUPADO")
            << std::setw(10) << (b.libre ? "-" : b.pid) << "\n";
    }
    out << std::string(38, '-') << "\n";
    out << "Bloques libres: " << bloquesLibres()
        << " | Bloques ocupados: " << bloquesOcupados() << "\n";
    out << "Memoria libre: " << memoriaLibreTotal()
        << " | Memoria ocupada: " << memoriaOcupadaTotal() << "\n";
    out << "Fragmentacion externa: " << fragmentacionExterna() << "\n";
    out << "Fragmentacion interna: 0 (particion dinamica exacta; no aplica "
           "en este esquema, ver informe)\n";
}


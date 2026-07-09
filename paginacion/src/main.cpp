// Simulador de Traduccion de Direcciones Virtuales a Fisicas
// Esquema: Paginacion de un nivel
//
// Uso:
//   ./paginacion                        -> abre el menu interactivo
//   ./paginacion entradas/archivo.txt   -> ejecuta un archivo de comandos
//
// Formato del archivo de comandos:
//   INICIAR <tamMemVirtual> <tamMemFisica> <tamPagina>
//   TRADUCIR <direccionVirtual>
//   LIBERAR <numPagina>
//   TABLA
//   CONFIG
//   # comentario

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "SimuladorPaginacion.h"

static void ejecutarLinea(SimuladorPaginacion*& sim, const std::string& linea,
                           std::ostream& out1, std::ostream& out2) {
    std::istringstream iss(linea);
    std::string cmd;
    iss >> cmd;
    if (cmd.empty() || cmd[0] == '#') return;

    std::ostringstream msg;

    if (cmd == "INICIAR") {
        int tv, tf, tp;
        iss >> tv >> tf >> tp;
        delete sim;
        sim = new SimuladorPaginacion(tv, tf, tp);
        msg << "[INICIAR] MV=" << tv << " MF=" << tf << " Pagina=" << tp << "\n";
        sim->imprimirConfiguracion(msg);
    } else if (cmd == "TRADUCIR") {
        int dir; iss >> dir;
        ResultadoTraduccion r = sim->traducir(dir);
        msg << "[TRADUCIR] DV=" << dir;
        if (r.exito) {
            msg << " -> Pagina=" << r.numPagina << " Offset=" << r.desplazamiento
                << " Marco=" << r.marco << " DF=" << r.direccionFisica
                << " | " << r.mensaje << "\n";
        } else {
            msg << " -> ERROR: " << r.mensaje << "\n";
        }
    } else if (cmd == "LIBERAR") {
        int pag; iss >> pag;
        bool ok = sim->liberarPagina(pag);
        msg << "[LIBERAR] Pagina " << pag << " -> "
            << (ok ? "liberada" : "no estaba mapeada") << "\n";
    } else if (cmd == "TABLA") {
        sim->imprimirTablaPaginas(msg);
    } else if (cmd == "CONFIG") {
        sim->imprimirConfiguracion(msg);
    } else {
        msg << "[AVISO] Comando no reconocido: " << cmd << "\n";
    }

    out1 << msg.str();
    out2 << msg.str();
}

static void procesarArchivo(SimuladorPaginacion*& sim, const std::string& rutaEntrada) {
    std::ifstream in(rutaEntrada);
    if (!in.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << rutaEntrada << "\n";
        return;
    }
    size_t barra = rutaEntrada.find_last_of("/\\");
    std::string base = (barra == std::string::npos) ? rutaEntrada : rutaEntrada.substr(barra + 1);
    std::string rutaSalida = "salidas/salida_" + base;

    std::ofstream out(rutaSalida);
    std::string linea;
    while (std::getline(in, linea)) {
        std::cout << ">> " << linea << "\n";
        out << ">> " << linea << "\n";
        ejecutarLinea(sim, linea, std::cout, out);
    }
    std::cout << "\nArchivo de salida generado en: " << rutaSalida << "\n";
}

static void menu() {
    std::cout << "\n=========================================\n";
    std::cout << " Simulador de Paginacion (1 nivel) - MMU\n";
    std::cout << "=========================================\n";
    std::cout << "1. Configurar memoria (virtual, fisica, tamano de pagina)\n";
    std::cout << "2. Traducir direccion virtual -> fisica\n";
    std::cout << "3. Ver tabla de paginas\n";
    std::cout << "4. Ver configuracion actual\n";
    std::cout << "5. Liberar una pagina\n";
    std::cout << "6. Cargar y ejecutar archivo de entrada\n";
    std::cout << "0. Salir\n";
    std::cout << "Seleccione una opcion: ";
}

int main(int argc, char* argv[]) {
    // Configuracion por defecto: 64 unidades de memoria virtual,
    // 32 unidades de memoria fisica, paginas de 4 unidades.
    SimuladorPaginacion* sim = new SimuladorPaginacion(64, 32, 4);

    std::cout << "Simulador de Paginacion de un nivel iniciado.\n";
    sim->imprimirConfiguracion(std::cout);

    if (argc > 1) {
        procesarArchivo(sim, argv[1]);
    }

    int opcion = -1;
    while (opcion != 0) {
        menu();
        if (!(std::cin >> opcion)) break;

        if (opcion == 1) {
            int tv, tf, tp;
            std::cout << "Tamano memoria virtual: "; std::cin >> tv;
            std::cout << "Tamano memoria fisica: "; std::cin >> tf;
            std::cout << "Tamano de pagina: "; std::cin >> tp;
            delete sim;
            sim = new SimuladorPaginacion(tv, tf, tp);
            sim->imprimirConfiguracion(std::cout);
        } else if (opcion == 2) {
            int dir;
            std::cout << "Direccion virtual a traducir: "; std::cin >> dir;
            ResultadoTraduccion r = sim->traducir(dir);
            if (r.exito) {
                std::cout << "Pagina=" << r.numPagina << " Offset=" << r.desplazamiento
                          << " Marco=" << r.marco << " -> Direccion fisica=" << r.direccionFisica
                          << "\n" << r.mensaje << "\n";
            } else {
                std::cout << "ERROR: " << r.mensaje << "\n";
            }
        } else if (opcion == 3) {
            sim->imprimirTablaPaginas(std::cout);
        } else if (opcion == 4) {
            sim->imprimirConfiguracion(std::cout);
        } else if (opcion == 5) {
            int pag;
            std::cout << "Numero de pagina a liberar: "; std::cin >> pag;
            bool ok = sim->liberarPagina(pag);
            std::cout << (ok ? "Pagina liberada.\n" : "La pagina no estaba mapeada.\n");
        } else if (opcion == 6) {
            std::string ruta;
            std::cout << "Ruta del archivo de entrada: "; std::cin >> ruta;
            procesarArchivo(sim, ruta);
        } else if (opcion != 0) {
            std::cout << "Opcion invalida.\n";
        }
    }

    delete sim;
    std::cout << "Fin del programa.\n";
    return 0;
}


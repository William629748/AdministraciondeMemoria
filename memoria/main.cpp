// Simulador de Asignacion de Memoria (First Fit / Best Fit / Worst Fit)i
//
// Uso:
//   ./memoria                        -> abre el menu interactivo
//   ./memoria entradas/archivo.txt   -> ejecuta un archivo de comandos y deja abierto el menu
//
// Formato del archivo de comandos (una instruccion por linea):
//   INICIAR <tamanoTotal> <ESTRATEGIA>      ESTRATEGIA = FF | BF | WF
//   ALLOC <pid> <tamano>
//   FREE <pid>
//   ESTADO
//   ESTRATEGIA <ESTRATEGIA>
//   # comentario

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "AdministradorMemoria.h"

static Estrategia parsearEstrategia(const std::string& s) {
    if (s == "FF" || s == "FIRST_FIT" || s == "1") return Estrategia::FIRST_FIT;
    if (s == "BF" || s == "BEST_FIT"  || s == "2") return Estrategia::BEST_FIT;
    if (s == "WF" || s == "WORST_FIT" || s == "3") return Estrategia::WORST_FIT;
    return Estrategia::FIRST_FIT;
}

// Ejecuta una linea de comando sobre el administrador una sola vez, y
// replica el texto resultante hacia dos streams (consola y archivo de
// salida). Es importante no invocar los metodos del administrador dos
// veces, porque ALLOC/FREE tienen efectos secundarios (mutan el estado
// de la memoria) y ejecutarlos por duplicado dañaria la simulacion.
static void ejecutarLinea(AdministradorMemoria*& admin, const std::string& linea,
                           std::ostream& out1, std::ostream& out2) {
    std::istringstream iss(linea);
    std::string cmd;
    iss >> cmd;

    if (cmd.empty() || cmd[0] == '#') return;

    std::ostringstream msg; // buffer con el resultado textual de la operacion

    if (cmd == "INICIAR") {
        int tam; std::string est;
        iss >> tam >> est;
        delete admin;
        admin = new AdministradorMemoria(tam, parsearEstrategia(est));
        msg << "[INICIAR] Memoria de tamano " << tam << " con estrategia "
            << AdministradorMemoria::nombreEstrategia(parsearEstrategia(est)) << "\n";
    } else if (cmd == "ESTRATEGIA") {
        std::string est; iss >> est;
        admin->setEstrategia(parsearEstrategia(est));
        msg << "[ESTRATEGIA] Cambiada a "
            << AdministradorMemoria::nombreEstrategia(parsearEstrategia(est)) << "\n";
    } else if (cmd == "ALLOC") {
        std::string pid; int tam;
        iss >> pid >> tam;
        bool ok = admin->asignar(pid, tam);
        msg << "[ALLOC] Proceso " << pid << " (" << tam << ") -> "
            << (ok ? "OK" : "FALLO: no hay bloque libre suficiente (sin memoria)") << "\n";
    } else if (cmd == "FREE") {
        std::string pid;
        iss >> pid;
        bool ok = admin->liberar(pid);
        msg << "[FREE] Proceso " << pid << " -> "
            << (ok ? "liberado" : "no encontrado") << "\n";
    } else if (cmd == "ESTADO") {
        admin->imprimirEstado(msg);
    } else {
        msg << "[AVISO] Comando no reconocido: " << cmd << "\n";
    }

    out1 << msg.str();
    out2 << msg.str();
}

static void procesarArchivo(AdministradorMemoria*& admin, const std::string& rutaEntrada) {
    std::ifstream in(rutaEntrada);
    if (!in.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << rutaEntrada << "\n";
        return;
    }

    std::string rutaSalida = "salidas/salida_" ;
    // usar solo el nombre base del archivo de entrada para la salida
    size_t barra = rutaEntrada.find_last_of("/\\");
    std::string base = (barra == std::string::npos) ? rutaEntrada : rutaEntrada.substr(barra + 1);
    rutaSalida += base;

    std::ofstream out(rutaSalida);
    std::string linea;
    while (std::getline(in, linea)) {
        std::cout << ">> " << linea << "\n";
        out << ">> " << linea << "\n";
        ejecutarLinea(admin, linea, std::cout, out);
    }
    std::cout << "\nArchivo de salida generado en: " << rutaSalida << "\n";
}

static void menu() {
    std::cout << "\n=========================================\n";
    std::cout << " Simulador de Asignacion de Memoria\n";
    std::cout << "=========================================\n";
    std::cout << "1. Iniciar/Reiniciar memoria\n";
    std::cout << "2. Cambiar estrategia (First/Best/Worst Fit)\n";
    std::cout << "3. Crear proceso (asignar memoria)\n";
    std::cout << "4. Liberar memoria de un proceso\n";
    std::cout << "5. Mostrar estado de la memoria\n";
    std::cout << "6. Cargar y ejecutar archivo de entrada\n";
    std::cout << "0. Salir\n";
    std::cout << "Seleccione una opcion: ";
}

int main(int argc, char* argv[]) {
    AdministradorMemoria* admin = new AdministradorMemoria(1024, Estrategia::FIRST_FIT);

    std::cout << "Simulador de Asignacion de Memoria iniciado.\n";
    std::cout << "Memoria por defecto: 1024 unidades, estrategia First Fit.\n";

    if (argc > 1) {
        procesarArchivo(admin, argv[1]);
    }

    int opcion = -1;
    while (opcion != 0) {
        menu();
        if (!(std::cin >> opcion)) { break; }

        if (opcion == 1) {
            int tam; std::string est;
            std::cout << "Tamano total de memoria: ";
            std::cin >> tam;
            std::cout << "Estrategia (FF=FirstFit, BF=BestFit, WF=WorstFit): ";
            std::cin >> est;
            delete admin;
            admin = new AdministradorMemoria(tam, parsearEstrategia(est));
            std::cout << "Memoria reiniciada.\n";
        } else if (opcion == 2) {
            std::string est;
            std::cout << "Nueva estrategia (FF/BF/WF): ";
            std::cin >> est;
            admin->setEstrategia(parsearEstrategia(est));
            std::cout << "Estrategia actualizada a "
                      << AdministradorMemoria::nombreEstrategia(admin->getEstrategia()) << "\n";
        } else if (opcion == 3) {
            std::string pid; int tam;
            std::cout << "PID del proceso: "; std::cin >> pid;
            std::cout << "Tamano solicitado: "; std::cin >> tam;
            bool ok = admin->asignar(pid, tam);
            std::cout << (ok ? "Asignacion exitosa.\n"
                             : "No hay memoria suficiente para este proceso.\n");
        } else if (opcion == 4) {
            std::string pid;
            std::cout << "PID del proceso a liberar: "; std::cin >> pid;
            bool ok = admin->liberar(pid);
            std::cout << (ok ? "Memoria liberada.\n" : "Proceso no encontrado.\n");
        } else if (opcion == 5) {
            admin->imprimirEstado(std::cout);
        } else if (opcion == 6) {
            std::string ruta;
            std::cout << "Ruta del archivo de entrada: ";
            std::cin >> ruta;
            procesarArchivo(admin, ruta);
        } else if (opcion != 0) {
            std::cout << "Opcion invalida.\n";
        }
    }

    delete admin;
    std::cout << "Fin del programa.\n";
    return 0;
}


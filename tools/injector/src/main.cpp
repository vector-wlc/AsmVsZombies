#include "injector.h"
#include <clocale>

int main(int argc, char* argv[]) {
    if (argc > 2) {
        printf("Usage: %s [dll_path]\n", argv[0]);
        return 1;
    }
    setlocale(LC_ALL, "");
    Process process;
    std::string dllPath = argc == 2 ? argv[1] : "bin/libavz.dll";
    if (process.SelectWindow())
        process.ManageDLL(dllPath);
    return 0;
}

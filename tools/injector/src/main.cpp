#include "injector.h"
#include <clocale>

int main() {
    setlocale(LC_ALL, "");
    Process process;
    if (process.SelectWindow())
        process.ManageDLL();
    return 0;
}

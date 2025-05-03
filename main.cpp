#include "JaDraw.h"
#include <iostream>
#include <iomanip>

template <int W, int H>
void printCanvas(const JaDraw<W, H>& jdrw)
{
    for (int y = 0; y < jdrw.height; ++y) {
        for (int x = 0; x < jdrw.width; ++x) {
            std::cout << std::hex << std::setw(8) << std::setfill('0') << jdrw.canvas[y * jdrw.width + x] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::dec << std::endl;
}

int main() {
    static JaDraw<4, 4> jdrw;
    printCanvas(jdrw);
    jdrw.drawPixel(0, 0, 1);
    printCanvas(jdrw);
    std::cout << "Press any key to continue..." << std::endl;
    std::cin.get();
    return 0;
}
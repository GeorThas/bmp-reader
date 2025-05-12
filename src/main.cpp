#include "bmp.h"
#include <cstdint>
#include <iostream>

int main(int argc, char *argv[]) {

  try {
    /*
    if (argc != 3) {
      throw std::invalid_argument{""
                                  ""
                                  ""};
    }
                                  */
    std::string inputBmpFile;
    std::string outputBmpFile;

    //==========

    std::cout << ">> Enter input BMP file name: "; // 1.bmp
    std::getline(std::cin, inputBmpFile);

    D::Bmp24 bmp{inputBmpFile};

    std::cout << "\nOriginal image:\n";
    // bmp.printToConsole();
    std::cout << bmp.BmpToString();

    // Рисование креста
    int w = bmp.getWidth(), h = bmp.getHeight();
    D::RGB24 black{0, 0, 0};
    bmp.drawLine(0, 0, w - 1, h - 1, black); // Диагональ 1
    bmp.drawLine(0, h - 1, w - 1, 0, black); // Диагональ 2

    std::cout << "\n\n";
    std::cout
        << "==============================================================";
    std::cout << "\n\n";
    // Вывод изменённого изображения
     std::cout << "\nModified image:\n";
    // bmp.printToConsole();
    std::cout << bmp.BmpToString();

    // Сохранение
    std::cout << ">> Enter output BMP file name: "; // 1.bmp
    std::getline(std::cin, outputBmpFile);

    if (!bmp.saveFile(outputBmpFile)) {
      std::cout << "failed to save image";
    }

  } catch (const std::exception &e) {
    std::cout << e.what();
  }
};
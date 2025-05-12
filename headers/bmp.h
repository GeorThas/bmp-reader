#include <climits>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>


namespace D {

struct RGB24 {
  uint8_t blue{255}, green{255}, red{255};
  inline static const uint32_t bitPerPixel24{24};
};

template <typename Pixel = RGB24> class Bmp {
public:
  using PixelType = Pixel;

  Bmp(uint32_t width, uint32_t height) noexcept(false);
  Bmp(const std::string &filename) noexcept(false);
  bool loadFile(const std::string &fileName) noexcept(false);
  bool saveFile(const std::string &fileName) const noexcept(false);
  uint32_t getWidth() const noexcept;
  uint32_t getHeight() const noexcept;
  void setPixel(uint32_t x, uint32_t y, const Pixel &color) noexcept;
  void drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2,
                Pixel &color = Pixel{}) noexcept(false);
  void clear() noexcept;
  void printToConsole() const noexcept;
  void BmpSymOutput(std::ostream &streamOut = std::cout, int prec = 0,
                       int ws = 0,
                       std::ios_base::fmtflags flag = std::ios::fixed |
                                                      std::ios::dec | std::ios::right,
                       const std::string &ch = "\n") const
      noexcept(false); //////флаги передавать как параметры
  std::string BmpToString(int prec = 0, int ws = 0,
                          std::ios_base::fmtflags flag = std::ios::fixed |
                                                         std::ios::dec |
                                                         std::ios::right,
                          const std::string &ch = "\n") const
      noexcept(false); // флаги передавать как параметры, а возвращать строку

  PixelType &operator[](uint32_t row, uint32_t col) noexcept(false);
  const PixelType &operator[](uint32_t row, uint32_t col) const noexcept(false);

private:
  bool validate() const noexcept;
  uint32_t calculateBytesPerRowWithPadding(uint32_t pixelsNum) const;

#pragma pack(push, 2)

  struct BITMAPFILEHEADER {
    uint16_t bfType{bmpSignature}; // 2
    uint32_t bfSize;               // 4
    uint16_t bfReserved1;          // 2 -
    uint16_t bfReserved2;          // 2 -
    uint32_t bfOffBits;            // 4
  } m_header;



  struct BITMAPINFOHEADER {
    uint32_t biSize{sizeof(BITMAPINFOHEADER)};
    int32_t biWidth{1};
    int32_t biHeight{1};
    uint16_t biPlanes{1};
    uint16_t biBitCount{defaultBitsPerPixel};
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter{defaultBitsPerPixel};
    int32_t biYPelsPerMeter{defaultBitsPerPixel};
    uint32_t biClrUsed{};
    uint32_t biClrImportant;
  } m_infoHeader;

  #pragma pack(pop)

  std::vector<uint8_t> m_data;

  inline static const uint16_t bmpSignature{0x4D42}; // bmp
  inline static const uint16_t defaultBitsPerPixel{RGB24::bitPerPixel24};
  inline static const uint32_t defaultPixelsPerMeter{3780};
  inline static const uint32_t padding{4};
};

using Bmp24 = Bmp<RGB24>;
} // namespace D

template <typename Pixel>
D::Bmp<Pixel>::Bmp(uint32_t width, uint32_t height) noexcept(false) {
  const auto bytesPerRow{(width * Pixel::bitsPerPixel) / CHAR_BIT};
  const auto bytesPerRowWidthPadding{calculateBytesPerRowWithPadding(width)};
  const auto dataOffset{sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)};
  const auto imageSize{bytesPerRowWidthPadding + height};
  m_header.bfSize = dataOffset + imageSize;
  m_header.bfOffBits = dataOffset;
  m_infoHeader.biWidth = width;
  m_infoHeader.biHeight = height;
  m_infoHeader.biSizeImage = imageSize;
  m_data.resize(imageSize, 0);
  clear();
}

template <typename Pixel>
D::Bmp<Pixel>::Bmp(const std::string &filename) noexcept(false) {
  if (!loadFile(filename)) {
    throw std::runtime_error{"failed to read file_1."};
  }
}

template <typename Pixel>
bool D::Bmp<Pixel>::loadFile(const std::string &fileName) noexcept(false) {
  std::ifstream file{fileName, std::ios::binary};
  if (file) {
    if (!file.read(reinterpret_cast<char *>(&m_header),
                   sizeof(BITMAPFILEHEADER))) {
      throw std::runtime_error{"failed to read file_2."};
    }
    if (!file.read(reinterpret_cast<char *>(&m_infoHeader),
                   sizeof(BITMAPINFOHEADER))) {
      throw std::runtime_error{"failed to read file_3."};
    }

    m_data.resize(m_infoHeader.biSizeImage);
    if (!file.read(reinterpret_cast<char *>(m_data.data()),
                   m_infoHeader.biSizeImage)) {
      throw std::runtime_error{"failed to read file_4."};
    }
    file.close();
    return true;
  }
  return false;
}

template <typename Pixel>
bool D::Bmp<Pixel>::saveFile(const std::string &fileName) const
    noexcept(false) {
  std::ofstream file{fileName, std::ios::binary};
  if (file) {
    if (!file.write(reinterpret_cast<const char *>(&m_header),
                    sizeof(BITMAPFILEHEADER))) {
      throw std::runtime_error{"failed to write file."};
    }
    if (!file.write(reinterpret_cast<const char *>(&m_infoHeader),
                    sizeof(BITMAPINFOHEADER))) {
      throw std::runtime_error{"failed to write file."};
    }

    // m_data.resize(m_infoHeader.biSizeImage);
    if (!file.write(reinterpret_cast<const char *>(m_data.data()),
                    m_infoHeader.biSizeImage)) {
      throw std::runtime_error{"failed to write file."};
    }
    file.close();
    return true;
  }
  return false;
}

template <typename Pixel> uint32_t D::Bmp<Pixel>::getWidth() const noexcept {
  return m_infoHeader.biWidth;
}

template <typename Pixel> uint32_t D::Bmp<Pixel>::getHeight() const noexcept {
  return m_infoHeader.biHeight;
}

template <typename Pixel>
void D::Bmp<Pixel>::setPixel(uint32_t x, uint32_t y,
                             const Pixel &color) noexcept {
  if (x >= 0 && y >= 0 && x < m_infoHeader.biWidth && m_infoHeader.biHeight)
    (*this)[y, x] = color;
}

template <typename Pixel>
void D::Bmp<Pixel>::drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2,
                             Pixel &color) noexcept(false) { //алгоритм Брезенхема
  int dx = abs(x2 - x1);
  int dy = -abs(y2 - y1);
  int sx = x1 < x2 ? 1 : -1;
  int sy = y1 < y2 ? 1 : -1;
  int err = dx + dy;

  while (true) {
    //setPixel(x1, y1+1, color);
    setPixel(x1, y1, color);
    //setPixel(x1, y1-1, color);
    if (x1 == x2 && y1 == y2)
      break;
    int e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x1 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y1 += sy;
    }
  }
}

template <typename Pixel> void D::Bmp<Pixel>::clear() noexcept {
  for (size_t i = 0; i < m_infoHeader.biHeight; ++i) {
    for (size_t j = 0; j < m_infoHeader.biWidth; ++j) {
      (*this)[i, j] = Pixel{};
    }
  }
}

//================================

template <typename Pixel>
void D::Bmp<Pixel>::BmpSymOutput(
    std::ostream &streamOut, int prec, int ws, std::ios_base::fmtflags flag,
    const std::string &ch) const noexcept(false) { //////флаги передавать как параметры

  streamOut.flags(flag);
  streamOut << std::setprecision(prec);

  for (uint32_t y = 0; y < m_infoHeader.biHeight; ++y) {
    for (uint32_t x = 0; x < m_infoHeader.biWidth; ++x) {
      const PixelType &p = (*this)[y, x];

      // Конвертация в яркость и символ
      // char c =
      //    static_cast<char>( 0.33 * p.red + 0.33 * p.green + 0.33 * p.blue);

      streamOut << std::setw(ws)
                << (static_cast<uint32_t>(p.red) == 255 ? ' ' : '#');
    }
    streamOut << ch;
  }
}

template <typename Pixel>
std::string D::Bmp<Pixel>::BmpToString(int prec, int,
                                       std::ios_base::fmtflags flag,
                                       const std::string &ch) const
    noexcept(false) { // флаги передавать как параметры, а возвращать строку

  std::stringstream streamOut;

  BmpSymOutput(streamOut);

  return streamOut.str();
}

//================================

template <typename Pixel> void D::Bmp<Pixel>::printToConsole() const noexcept {
  for (uint32_t y = 0; y < m_infoHeader.biHeight; ++y) {
    for (uint32_t x = 0; x < m_infoHeader.biWidth; ++x) {
      const PixelType &p = (*this)[y, x];

      // Конвертация в яркость и символ
      // char c =
      //    static_cast<char>( 0.33 * p.red + 0.33 * p.green + 0.33 * p.blue);

      std::cout << (static_cast<uint32_t>(p.green) >= 50 ? ' ' : '#');
    }
    std::cout << '\n';
  }
}

template <typename Pixel>
const D::Bmp<Pixel>::PixelType &D::Bmp<Pixel>::operator[](uint32_t row,
                                                          uint32_t col) const
    noexcept(false) {

  if (row >= m_infoHeader.biHeight || col >= m_infoHeader.biWidth) {
    throw std::out_of_range{""};
  }
  const auto reversedRow{m_infoHeader.biHeight - row - 1};
  const auto bytesPerRowWithPadding{
      calculateBytesPerRowWithPadding(m_infoHeader.biWidth)};

  return reinterpret_cast<const Pixel &>(
      m_data[reversedRow * bytesPerRowWithPadding +
             col * m_infoHeader.biBitCount / CHAR_BIT]);
}

template <typename Pixel>
D::Bmp<Pixel>::PixelType &
D::Bmp<Pixel>::operator[](uint32_t row, uint32_t col) noexcept(false) {
  return const_cast<Bmp<Pixel>::PixelType &>(
      const_cast<const Bmp<Pixel> *>(this)->operator[](row, col));
}

template <typename Pixel> bool D::Bmp<Pixel>::validate() const noexcept {
  return (m_header.bfType == bmpSignature && m_infoHeader.biPlanes == 1 &&
          m_infoHeader.biBitCount == defaultBitsPerPixel &&
          m_infoHeader.biCompression == 0);
}

template <typename Pixel>
uint32_t
D::Bmp<Pixel>::calculateBytesPerRowWithPadding(uint32_t pixelsNum) const {
  const auto bytes{pixelsNum * Pixel::bitPerPixel24 / CHAR_BIT};
  const auto nearestDivisibleByPadding{padding *
                                       ((bytes + padding - 1) / padding)};
  return nearestDivisibleByPadding;
}

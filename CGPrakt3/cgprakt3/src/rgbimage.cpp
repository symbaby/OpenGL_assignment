//
// Created by boery on 21.03.2022.
//

#include "rgbimage.h"
#include "color.h"

#include "assert.h"
#include "string.h"
#include "stdio.h"

RGBImage::RGBImage(unsigned int Width, unsigned int Height) : m_Width(Width), m_Height(Height) {
    // Ein Bild wird beschrieben von hoehe x breite
    m_Image = new Color[Width * Height];
}

RGBImage::~RGBImage() {
    delete (m_Image);
}

void RGBImage::setPixelColor(unsigned int x, unsigned int y, const Color &c) {
    if (x < 0 || x > width() || y < 0 || y > height()) {
        throw std::invalid_argument("falsche Farbwerte erhalten");
    } else {
        m_Image[y * width() + x] = c;
    }
}

const Color &RGBImage::getPixelColor(unsigned int x, unsigned int y) const {
    if (x < 0 || x > width() || y < 0 || y > height()) {
        throw std::invalid_argument("falsche Farbwerte erhalten");
    } else {
        return m_Image[y * width() + x];
    }
}

unsigned int RGBImage::width() const {

    return m_Width;
}

unsigned int RGBImage::height() const {

    return m_Height;
}

unsigned char RGBImage::convertColorChannel(float v) {
    if (v < 0.0f) {
        v = 0.0f;
    }

    if (v > 1.0f) {
        v = 1.0f;
    }

    return (unsigned char) (v * 255);
}

// Ansatz fuer die Loesung: https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries
bool RGBImage::saveToDisk(const char *Filename) {
    FILE *f;
    unsigned char *img = nullptr;

    // Wieso 54: Antwort -> 14 byte Bitmap + File Header, 40 byte Bitmap Info Header, 3 Byte RGB
    unsigned int filesize = 54 + 3 * width() * height();  //w is your image width, h is image height, both int


    // Speicher reservieren
    img = (unsigned char *) malloc(3 * width() * height());
    memset(img, 0, 3 * width() * height());

    // Durch Bild Pixel fuer Pixel durchlaufen
    for (int i = 0; i < width(); i++) {
        for (int j = 0; j < height(); j++) {

            // variablen fuer traversierung preppen
            unsigned int x = i;
            unsigned int y = (height() - 1 - j);

            // Wir bringen die RGB float Werte von 0.0f - 1.0f in die 0-255 range
            unsigned char r = convertColorChannel(this->getPixelColor(i, j).R);
            unsigned char g = convertColorChannel(this->getPixelColor(i, j).G);
            unsigned char b = convertColorChannel(this->getPixelColor(i, j).B);


            // Cap fuer Schrottwerte
            if (r > 255) r = 255;
            if (g > 255) g = 255;
            if (b > 255) b = 255;

            img[(x + y * width()) * 3 + 2] = (unsigned char) (r);
            img[(x + y * width()) * 3 + 1] = (unsigned char) (g);
            img[(x + y * width()) * 3 + 0] = (unsigned char) (b);
        }
    }

    // (BITMAPFILEHEADER) Groesse 14 Byte, Jede Zelle = 1 Byte
    unsigned char bmpfileheader[14] = {
            'B',        // bfType
            'M',        // bfType
            0,          // bfSize
            0,          // bfSize
            0,          // bfSize
            0,          // bfSize
            0,          // bfReserved
            0,          // bfReserved
            0,          // bfReserved
            0,          // bfReserved
            54,        // bfOffBits
            0,         // bfOffBits
            0,         // bfOffBits
            0          // bfOffBits
    };

    unsigned char bmpinfoheader[40] = {
            40,         // biSize
            0,          // biSize
            0,          // biSize
            0,          // biSize
            0,          // biWidth
            0,          // biWidth
            0,          // biWidth
            0,          // biWidth
            0,          // biHeight
            0,          // biHeight
            0,         // biHeight
            0,         // biHeight
            1,         // biPlanes
            0,         // biPlanes
            24,
            0
    };

    unsigned char bmppad[3] = {0, 0, 0};

    bmpfileheader[2] = (unsigned char) (filesize);          // bfsize
    bmpfileheader[3] = (unsigned char) (filesize >> 8);     // bfsize
    bmpfileheader[4] = (unsigned char) (filesize >> 16);    // bfsize
    bmpfileheader[5] = (unsigned char) (filesize >> 24);    // bfsize

    bmpinfoheader[4] = (unsigned char) (width());           // biWidth
    bmpinfoheader[5] = (unsigned char) (width() >> 8);      // biWidth
    bmpinfoheader[6] = (unsigned char) (width() >> 16);     // biWidth
    bmpinfoheader[7] = (unsigned char) (width() >> 24);     // biWidth

    /**
     * https://de.wikipedia.org/wiki/Windows_Bitmap @BITMAPINFOHEADER (Größe: 40 Byte)
     *
     * Ist der Wert positiv, so ist die Bitmap eine sogenannte "bottom-up"-Bitmap
     * (die Bilddaten beginnen mit der untersten und enden mit der obersten Bildzeile)
     *
     * Ist der Wert negativ, so ist die Bitmap eine “top-down”-Bitmap
     * (die Bilddaten beginnen mit der obersten und enden mit der untersten Bildzeile).
     *
     * */

    bmpinfoheader[8] = (unsigned char) (-1 * height());  // Hier muessen die heights eine negative Zahl sein!
    bmpinfoheader[9] = (unsigned char) (-1 * height() >> 8);
    bmpinfoheader[10] = (unsigned char) (-1 * height() >> 16);
    bmpinfoheader[11] = (unsigned char) (-1 * height() >> 24);


    f = fopen(Filename, "wb");
    fwrite(bmpfileheader, 1, 14, f);
    fwrite(bmpinfoheader, 1, 40, f);
    for (int i = 0; i < height(); i++) {
        fwrite(img + (width() * (height() - i - 1) * 3), 3, width(), f);
        fwrite(bmppad, 1, (4 - (width() * 3) % 4) % 4, f);
    }

    // Speicher freigeben
    free(img);

    // geoffnete Dateien muessen geschlossen werden.
    fclose(f);

    return true;
}
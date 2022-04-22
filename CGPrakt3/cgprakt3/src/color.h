//
// Created by boery on 21.03.2022.
//

#ifndef CGPRAKT2_COLOR_H
#define CGPRAKT2_COLOR_H

#include <iostream>

class Color {
public:
    float R;
    float G;
    float B;

    Color();

    Color(float r, float g, float b);

    Color operator*(const Color &c) const;

    Color operator*(const float Factor) const;

    Color operator+(const Color &c) const;

    Color &operator+=(const Color &c);

    // Fuer einfache Tests
    friend std::ostream& operator<<(std::ostream &s, const Color &out);

};

#endif //CGPRAKT2_COLOR_H

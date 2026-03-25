//
// Created by Curry on 2026-01-28.
//

#ifndef PROJECT_VECTOR2D_H
#define PROJECT_VECTOR2D_H
#include "iostream"

class Vector2D {
    public:
    float x = 0.0f;
    float y= 0.0f;

    Vector2D() {
        x = 0.0f;
        y = 0.0f;
    };

    Vector2D(float x, float y): x(x), y(y) {};

    //member operator function
    //Vector 2D has to be on left, float on the right
    Vector2D operator*(float scaler) const;
    //Reverse: Vector on right, float on left
    //need to make non-member function
    friend Vector2D operator*(float scaler, Vector2D& vec);


    //Extend with += and -= (With another Vector)
    Vector2D& operator+=(const Vector2D& vec);
    Vector2D& operator-=(const Vector2D& vec);

    //Extend with *= and /= Multiply with a float
    Vector2D& operator*=(float scaler);
    Vector2D& operator/=(float scaler);

    //Extend with + and - another vector
    Vector2D operator+(const Vector2D& vec) const;
    Vector2D operator-(const Vector2D& vec) const;

    //Extend with * and / another float
    // * is defined above
    Vector2D operator/(float scaler) const;
    friend Vector2D operator/(float scaler, Vector2D& vec);

    //Extend == and != with another vecotr
    bool operator==(const Vector2D& vec) const;
    bool operator!=(const Vector2D& vec) const;

    //Extend for -
    Vector2D operator-() const;

    friend std::ostream& operator<<(std::ostream& os, const Vector2D& vec);

    Vector2D& normalize();

};

inline std::ostream& operator<<(std::ostream& os, const Vector2D& vec) {
    os << "(" << static_cast<int>(vec.x) << ", " << static_cast<int>(vec.y) << ")";
    return os;
}



#endif //PROJECT_Vconst Vector2D &OR2D_H
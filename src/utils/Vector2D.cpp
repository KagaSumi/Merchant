//
// Created by Curry on 2026-01-28.
//

#include "Vector2D.h"

#include <math.h>

//Member for right Left
Vector2D Vector2D::operator*(float scaler) const {
    return Vector2D(x * scaler, y * scaler);
}

//Non-member for left right
Vector2D operator*(float scaler, Vector2D& vec) {
    return Vector2D(vec.x * scaler, vec.y * scaler);
}

Vector2D& Vector2D::operator+=(const Vector2D& vec){
    this->x +=  vec.x;
    this->y +=  vec.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& vec) {
    this->x -=  vec.x;
    this->y -=  vec.y;
    return *this;
}

Vector2D & Vector2D::operator*=(float scaler) {
    this->x *= scaler;
    this->y *= scaler;
    return *this;
}

Vector2D& Vector2D::operator/=(float scaler) {
    this->x /= scaler;
    this->y /= scaler;
    return *this;
}

Vector2D Vector2D::operator+(const Vector2D &vec) const {
    return Vector2D(x + vec.x, y + vec.y);
}

Vector2D  Vector2D::operator-(const Vector2D &vec) const {
    return Vector2D(x- vec.x, y-vec.y);
}

Vector2D Vector2D::operator/(float scaler) const {
    return Vector2D(x / scaler, y / scaler);
}

bool Vector2D::operator==(const Vector2D &vec) const {
    return x == vec.x && y == vec.y;
}

bool Vector2D::operator!=(const Vector2D &vec) const {
    return x == vec.x && y == vec.y;
}

Vector2D Vector2D::operator-() const {
    return Vector2D(-x, -y);
}

Vector2D &Vector2D::normalize() {
    float length = std::sqrt(x * x + y * y);

    if (length > 0) {
        this -> x /= length;
        this -> y /= length;
    }
    return *this;
}
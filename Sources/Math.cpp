#include "Math.h"

Vector3 Vector3::GetNormalized() const
{
    return *this / this->Length();
}

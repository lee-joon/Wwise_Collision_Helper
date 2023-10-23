#pragma once
#include <AK/SoundEngine/Common/IAkPlugin.h>

#include <array>
#include <cmath>
#include <limits>
#include <algorithm>

class SinCosTanLookupTable {
public:
    static constexpr AkUInt32 TABLE_SIZE = 1024;
    static constexpr AkReal32 PI = 3.14159265358979323846f;
    static constexpr AkReal32 TWO_PI = 2.0f * PI;

    SinCosTanLookupTable() {
        for (AkUInt32 i = 0; i < TABLE_SIZE; ++i) {
            AkReal32 angle = static_cast<AkReal32>(i) / TABLE_SIZE * TWO_PI;
            sinTable[i] = std::sin(angle);
            cosTable[i] = std::cos(angle);
            tanTable[i] = std::tan(angle);
        }
    }

    AkReal32 sin(AkReal32 angle) const { return lookup(angle, sinTable); }
    AkReal32 cos(AkReal32 angle) const { return lookup(angle, cosTable); }
    AkReal32 tan(AkReal32 angle) const {
        AkReal32 cosVal = cos(angle);
        if (std::abs(cosVal) < 1e-6f) {
            return (cosVal < 0.0f) ? -std::numeric_limits<AkReal32>::infinity() : std::numeric_limits<AkReal32>::infinity();
        }
        return sin(angle) / cosVal;
    }

private:
    std::array<AkReal32, TABLE_SIZE> sinTable{};
    std::array<AkReal32, TABLE_SIZE> cosTable{};
    std::array<AkReal32, TABLE_SIZE> tanTable{};

    AkReal32 lookup(AkReal32 angle, const std::array<AkReal32, TABLE_SIZE>& table) const {
        AkReal32 position = fmod(angle, 1.0f) * TABLE_SIZE;
        AkUInt32 index = static_cast<AkUInt32>(position);
        AkReal32 frac = position - index;
        AkReal32 a = table[index];
        AkReal32 b = table[(index + 1) % TABLE_SIZE];
        return a * (1 - frac) + b * frac;
    }
};

class DbToLinearLookupTable {
public:
    static constexpr size_t TABLE_SIZE = 1024;
    static constexpr AkReal32 MIN_DB = -60.0f;
    static constexpr AkReal32 MAX_DB = 20.0f;

    DbToLinearLookupTable() {
        for (size_t i = 0; i <= TABLE_SIZE; ++i) {
            AkReal32 normalized = static_cast<AkReal32>(i) / TABLE_SIZE;
            AkReal32 dBValue = MIN_DB + normalized * (MAX_DB - MIN_DB);
            table[i] = std::pow(10.0f, dBValue / 10.0f);
        }
    }

    AkReal32 dBToLinear(AkReal32 dBValue) const {
        AkReal32 normalized = (dBValue - MIN_DB) / (MAX_DB - MIN_DB);
        normalized = std::clamp(normalized, 0.0f, 1.0f);

        AkReal32 position = normalized * TABLE_SIZE;
        AkUInt32 index = static_cast<AkUInt32>(position);

        if (index >= TABLE_SIZE) return table[TABLE_SIZE];

        AkReal32 fraction = position - index;
        return table[index] * (1.0f - fraction) + table[index + 1] * fraction;
    }

private:
    std::array<AkReal32, TABLE_SIZE + 1> table;
};

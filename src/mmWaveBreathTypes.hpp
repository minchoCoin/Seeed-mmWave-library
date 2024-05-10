/**
 * @file mmWaveBreath.h
 * @date  09 May 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef MMWAVEBREATHTYPES_H
#define MMWAVEBREATHTYPES_H
#include <Arduino.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Macro to define an enumeration and its corresponding mask
#define DEFINE_TYPE(name, enumValue, maskShift) \
    name        = enumValue,                    \
    MASK_##name = (1 << maskShift)

typedef enum TypeHeartBreath {
    DEFINE_TYPE(HeartBreathPhaseType, 0x0A13, 0),
    DEFINE_TYPE(BreathRateType, 0x0A14, 1),
    DEFINE_TYPE(HeartRateType, 0x0A15, 2),
    DEFINE_TYPE(HeartBreathDistanceType, 0x0A16, 3)
} TypeHeartBreath;

uint32_t TypeHeartBreathToMask(TypeHeartBreath type) {
    switch (type) {
        case HeartBreathPhaseType:
            return MASK_HeartBreathPhaseType;
        case BreathRateType:
            return MASK_BreathRateType;
        case HeartRateType:
            return MASK_HeartRateType;
        case HeartBreathDistanceType:
            return MASK_HeartBreathDistanceType;
        default:
            return 0;
    }
}

#ifdef __cplusplus
} /*extern "C"*/
#endif


class BaseData {
  public:
    virtual ~BaseData()                     = default;
    virtual void print() const              = 0;
    virtual TypeHeartBreath getType() const = 0;
    virtual uint32_t getMask() const        = 0;
};

class HeartBreath : public BaseData {
    float total_phase;
    float breath_phase;
    float heart_phase;

  public:
    HeartBreath(float tp, float bp, float hp) : total_phase(tp), breath_phase(bp), heart_phase(hp) {}
    void print() const override {
        Serial.printf("Total Phase: %f, Breath Phase: %f, Heart Phase: %f\n", total_phase, breath_phase, heart_phase);
    }
    TypeHeartBreath getType() const override {
        return HeartBreathPhaseType;
    }

    uint32_t getMask() const override {
        return MASK_HeartBreathPhaseType;
    }
};

class BreathRate : public BaseData {
    float breath_rate;

  public:
    BreathRate(float br) : breath_rate(br) {}
    void print() const override {
        Serial.printf("Breath Rate: %f\n", breath_rate);
    }
    TypeHeartBreath getType() const override {
        return BreathRateType;
    }
    uint32_t getMask() const override {
        return MASK_BreathRateType;
    }
};

class HeartRate : public BaseData {
    float heart_rate;

  public:
    HeartRate(float hr) : heart_rate(hr) {}
    void print() const override {
        Serial.printf("Heart Rate: %f\n", heart_rate);
    }
    TypeHeartBreath getType() const override {
        return HeartRateType;
    }
    uint32_t getMask() const override {
        return MASK_HeartRateType;
    }
};

class HeartBreathDistance : public BaseData {
    uint32_t flag;
    float range;

  public:
    HeartBreathDistance(uint32_t f, float r) : flag(f), range(r) {}
    void print() const override {
        Serial.printf("Range: %f\n", range);
    }
    TypeHeartBreath getType() const override {
        return HeartBreathDistanceType;
    }

    uint32_t getMask() const override {
        return MASK_HeartBreathDistanceType;
    }
};

#endif /*MMWAVEBREATHTYPES_H*/

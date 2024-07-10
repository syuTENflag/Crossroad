#pragma once
#include "DxLib.h"
#include <vector>

struct Particle {
    VECTOR position;
    VECTOR velocity;
    int life;
};

class Effect {
private:
    std::vector<Particle> particles;
    int maxParticles;
    int particleLife;

public:
    Effect(int maxParticles, int particleLife);
    void update();
    void draw();
    void emit(VECTOR position);
};

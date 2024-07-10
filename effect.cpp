#include "effect.h"

Effect::Effect(int maxParticles, int particleLife)
    : maxParticles(maxParticles), particleLife(particleLife) {}

void Effect::update() {
    for (auto& particle : particles) {
        particle.position = VAdd(particle.position, particle.velocity);
        particle.life--;
    }
    particles.erase(
        std::remove_if(particles.begin(), particles.end(), [](const Particle& p) { return p.life <= 0; }),
        particles.end()
    );
}

void Effect::draw() {
    for (const auto& particle : particles) {
        DrawSphere3D(particle.position, 5.0f, 10, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
    }
}

void Effect::emit(VECTOR position) {
    for (int i = 0; i < maxParticles; ++i) {
        Particle p;
        p.position = position;
        p.velocity = VGet((float)(rand() % 100 - 50) / 10.0f, (float)(rand() % 100 - 50) / 10.0f, (float)(rand() % 100 - 50) / 10.0f);
        p.life = particleLife;
        particles.push_back(p);
    }
}

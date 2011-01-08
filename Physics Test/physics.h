#ifndef PHYSICS_H
#define PHYSICS_H

#include "game_systems.h"
#include "box2d.h"

class CPhysics : public CGameSystem
{
public:
    bool Init();
    void Destroy();

    void Update();
    void Render();

    static float GetScale() { return 64.0f; }
    static float GetScale(float length) { return length/64.0f; }

protected:
private:

    b2Body  *m_pBall;
    b2Body  *m_pWalls;
    b2World *m_pWorld;
};

#endif // PHYSICS_H
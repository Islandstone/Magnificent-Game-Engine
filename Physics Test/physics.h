#ifndef PHYSICS_H
#define PHYSICS_H

#include "game_systems.h"
#include "box2d.h"

class CPhysics : public CGameSystem
{
public:
protected:
private:

    b2World *m_pWorld;
};

#endif // PHYSICS_H
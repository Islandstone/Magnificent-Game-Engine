#ifndef GAME_SYSTEMS_H
#define GAME_SYSTEMS_H

#include "base.h"

class IGameSystem
{
public:
    virtual bool Init() = 0;
    virtual void Destroy() = 0;

    virtual void Update() = 0;
    virtual void PostRender() = 0;

    virtual const String GetName() = 0;
};

class CGameSystem : public IGameSystem
{
public:
    virtual bool Init() { return true; }
    virtual void Destroy() {}

    virtual void Update() {}
    virtual void PostRender() {}

    virtual const String GetName() { return String(L"---BASE---"); }
};

#endif // GAME_SYSTEMS_H
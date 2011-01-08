#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#if 0
#define REGISTER_GAME_CLASS( game_class ) \
class C##game_class##_Factory : public IGameFactory      \
{                                                       \
public:                                                 \
    C##game_class##_Factory()                            \
    {                                                   \
        g_pGameFactory = this;            \
        m_ptr = NULL;                                   \
    }                                                   \
    IGameInterface* Create()                            \
    {                                                   \
        m_ptr = new C##game_class##();                 \
        return m_ptr;                                   \
    }                                                   \
    void Destroy()                                      \
    {                                                   \
        if (m_ptr)                                      \
        {                                               \
            delete m_ptr;                               \
            m_ptr = NULL;                               \
        }                                               \
    }                                                   \
private:                                                \
    IGameInterface *m_ptr;                              \
};                                                      \
C##game_class##_Factory ##game_class##_FactoryObject;
#endif


class IGameInterface 
{
public:
    virtual bool Init() = 0;
    virtual void Shutdown() = 0;
    virtual void Destroy() = 0;

    virtual void PreStart() = 0;
    virtual void Start() = 0;

    virtual void Think() = 0;
    virtual void Render() = 0;

    virtual void PreThink() = 0;
    virtual void PostThink() = 0;

    virtual void PreRender() = 0;
    virtual void PostRender() = 0;
};

class CGameInterface : public IGameInterface
{
public:
    virtual bool Init(){return true;}
    virtual void Shutdown(){}
    virtual void Destroy() {}

    virtual void PreStart(){}
    virtual void Start(){}

    virtual void Think(){}
    virtual void Render(){}

    virtual void PreThink(){}
    virtual void PostThink(){}

    virtual void PreRender(){}
    virtual void PostRender(){}
};

class IGameFactory
{
public:
    virtual IGameInterface* Create()    = 0;
    virtual void            Destroy()   = 0;
};

extern IGameFactory* g_pGameFactory;
//extern IGameInterface *_g_pGameInterface;

#endif // GAMEINTERFACE_H
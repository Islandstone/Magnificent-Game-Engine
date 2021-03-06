#ifndef GAME_H
#define GAME_H

#include "sprite.h"
#include "tictactoe_ai.h"
#include "board.h"

class CGame : public CGameInterface
{
public:

    virtual bool Init();
    virtual void Destroy();

    virtual void PreStart();
    virtual void Start();

    virtual void Think();
    virtual void Render();

    virtual void PreThink(){}
    virtual void PostThink(){}

    virtual void PreRender(){}
    virtual void PostRender(){}

    void PlayHuman();
    void PlayAI();

protected:

    void Reset();

private:
    
    int                 m_iCurrentPlayer;
    CBoard             *m_pBoard;
    CTicTacToeAI       *m_pAI;
    float               m_flLastThinkTime;
};

class CGame_Factory : public IGameFactory    
{             
public:                                      
    CGame_Factory()                          
    {                                        
        g_pGameFactory = this;  
        m_ptr = NULL;                         
    }                                         
    IGameInterface* Create()                  
    {                                         
        m_ptr = new CGame();                 
        return m_ptr;                        
    }                                                   
    void Destroy()                                      
    {                                                   
        if (m_ptr != NULL)                                      
        {                                               
            delete m_ptr;                               
            m_ptr = NULL;                               
        }                                               
    }                                                   
private:                                                
    IGameInterface *m_ptr;                              
};     
#endif // GAME_H
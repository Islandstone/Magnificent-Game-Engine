#ifndef SPRITE_H
#define SPRITE_H

#include <d3dx9.h>
#include "base.h"
#include "smart_ptr.h"
#include <vector>
#include "texture.h"

class CSprite
{
public:
    CSprite();
    CSprite( String texturename );
    ~CSprite() 
    {
        g_pTextureLoader->Release(m_strFilename);
    }

    void SetTexture(String texturename);

    void SetX(float x);
    void SetY(float y);

    float GetX() { return m_vecPosition.x; }
    float GetY() { return m_vecPosition.y; }

    void SetPosition(float x, float y);
    void SetRotation(float rot);
    void SetScale( float scale );

    float GetRotation() { return m_flRotation; }
    float GetScale() { return m_flScale; }

    void SetRenderColorUniform(int i) { m_clrRenderColor = D3DCOLOR_ARGB(i,i,i,i); }

    //void Init();
    //void PostThink();

    //operator ID3DXSprite*() { return m_pSprite; }

    void Render();
protected:
    void GenerateMatrix();
private:

    D3DCOLOR                    m_clrRenderColor;
    D3DXVECTOR3                 m_vecPosition;
    float                       m_flRotation;
    float                       m_flScale;

    CTexture                    *m_pTexture;

    String                      m_strFilename;

    bool                        m_bShouldRender;
    bool                        m_bTransformIsDirty;
    D3DXMATRIX                  m_matTransform;
    CComPtr<ID3DXSprite>                 m_pSprite;

    RECT                        m_rtClippingRect;
};

#endif // SPRITE_H
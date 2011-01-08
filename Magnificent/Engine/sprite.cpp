#include "sprite.h"
#include "engine.h"

CSprite::CSprite()
{
    m_bShouldRender = true;
    m_bTransformIsDirty = true;

    m_vecPosition.x = 0;
    m_vecPosition.y = 0;
    m_vecPosition.z = 0;

    m_flRotation = 0.0f;
    m_flScale = 1.0f;

    m_clrRenderColor = 0xffffffff;

    D3DXCreateSprite( Engine()->GetDevice(), &m_pSprite);

    Engine()->AddObject(m_pSprite);
}

CSprite::CSprite( String texturename )
{
    m_bShouldRender = true;
    m_bTransformIsDirty = true;

    m_vecPosition.x = 0;
    m_vecPosition.y = 0;
    m_vecPosition.z = 0;

    m_flRotation = 0.0f;
    m_flScale = 1.0f;

    D3DXCreateSprite( Engine()->GetDevice(), &m_pSprite);

    SetTexture(texturename);

    Engine()->AddObject(m_pSprite);
}

void CSprite::SetPosition(float x, float y)
{
    m_bTransformIsDirty = true;
    m_vecPosition.x = x;
    m_vecPosition.y = y;
    m_vecPosition.z = 0.0f;
}

void CSprite::SetRotation(float rot)
{
    m_bTransformIsDirty = true;
    m_flRotation = rot;
}

void CSprite::SetScale(float scale)
{
    m_bTransformIsDirty = true;
    m_flScale = scale;
}

void CSprite::SetX(float x)
{
    m_bTransformIsDirty = true;
    m_vecPosition.x = x;
}

void CSprite::SetY(float y)
{
    m_bTransformIsDirty = true;
    m_vecPosition.y = y;
}

void CSprite::SetTexture( String texturename )
{
    m_pTexture = g_pTextureLoader->LoadTexture(texturename);
    m_strFilename = texturename;

    if (m_pTexture == NULL || m_pTexture->m_pTexture == NULL)
    {
        Engine()->Debug(L"Failed to load " + texturename + L"\n");
        return;
    }

    SetRect(&m_rtClippingRect, 0,0, m_pTexture->GetWidth(), m_pTexture->GetHeight() );
}

void CSprite::GenerateMatrix()
{
    D3DXMATRIX matTranslate;
    D3DXMATRIX matRotate;
    D3DXMATRIX matScale;

    D3DXMatrixIdentity(&m_matTransform);

    D3DXMatrixScaling(&matScale, m_flScale, m_flScale, 1.0f);
    D3DXMatrixTranslation(&matTranslate, m_vecPosition.x, m_vecPosition.y, 0.0f);
    D3DXMatrixRotationZ(&matRotate, D3DXToRadian(m_flRotation));

    D3DXMatrixMultiply(&matTranslate, &matScale, &matTranslate);
    D3DXMatrixMultiply(&m_matTransform, &matRotate, &matTranslate);

    m_pSprite->SetTransform(&m_matTransform);
    m_bTransformIsDirty = false;
}

void CSprite::Render()
{
    if (m_pTexture == NULL)
    {
        return;
    }

    //if (m_bTransformIsDirty)
    {
        GenerateMatrix();
    }

    D3DXVECTOR3 center(m_pTexture->GetWidth()*0.5f, m_pTexture->GetHeight()*0.5f, 0.0f);
    
    // TODO: Error check
    m_pSprite->Begin( D3DXSPRITE_OBJECTSPACE );
    m_pSprite->Draw(m_pTexture->GetTexture(), &m_rtClippingRect, &center, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), m_clrRenderColor );
    m_pSprite->End();   
}
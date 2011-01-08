#include "texture.h"
#include "engine.h"

CTextureLoader g_TextureLoader;

CTextureLoader *g_pTextureLoader = &g_TextureLoader;

CTexture* CTextureLoader::LoadTexture(String textureName)
{
    CTexture *pTexture = Get(textureName);

    if (pTexture != NULL)
    {
        return pTexture;
    }

    pTexture = new CTexture();
    pTexture->m_sName = textureName;

    HRESULT res = D3DXCreateTextureFromFileEx( Engine()->GetDevice(), textureName.c_str(), 0, 0, 1, 0, 
        D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 
        0, &pTexture->m_sImageInfo, NULL, &pTexture->m_pTexture);

    if (FAILED(res))
    {
        return NULL;
    }

    Insert(textureName, pTexture);

    return pTexture;
}
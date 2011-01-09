#ifndef TEXTURE_H
#define TEXTURE_H

#include "base.h"

#include <map>
#include <utility>
//#include "engine.h"
#include <d3dx9core.h>
#include <memory>
#include <vector>
#include "smart_ptr.h"

using namespace std;

class CTextureLoader;

class CTexture
{
public:
    CTexture() {};

    ~CTexture() 
    {
        if (m_pTexture)
        {
            m_pTexture->Release();
            m_pTexture = NULL;
        }
    }

    friend CTextureLoader;

    bool    IsLoaded() { return m_pTexture != NULL; }

    IDirect3DTexture9*   GetTexture() { return m_pTexture; }

    int     GetWidth() { return m_sImageInfo.Width; }
    int     GetHeight() { return m_sImageInfo.Height; }
    int     GetDepth() { return m_sImageInfo.Depth; }

    D3DFORMAT               GetFormat() { return m_sImageInfo.Format; }
    D3DRESOURCETYPE         GetResourceType() { return m_sImageInfo.ResourceType; }
    D3DXIMAGE_FILEFORMAT    GetFileFormat() { return m_sImageInfo.ImageFileFormat; }

    String  GetName() { return m_sName; }

    IDirect3DTexture9           *m_pTexture;
    D3DXIMAGE_INFO               m_sImageInfo;    

protected:
   
private:
    String                       m_sName;
};

class CTextureLoader
{
public:
    typedef std::map<String, CTexture*> Array;
    typedef std::pair<String, CTexture*> Pair;

    CTextureLoader()
    {
    }
    ~CTextureLoader()
    {
    }

    CTexture* LoadTexture(String textureName);

    void Reset()
    {
        // Textures are released by the sprites that use them

        /*
        vector<CTexture*>::iterator iter = m_vTextures.begin();

        while ( iter != m_vTextures.end() )
        {
            CTexture *pTexture = *iter;

            if (pTexture != NULL)
            {
                delete pTexture;
            }

            iter++;
        }
        */

        m_vTextures.clear();
    }

    void Release(String name)
    {
        if (m_vTextures.size() == 0)
        {
            return;
        }

        vector<CTexture*>::iterator iter = m_vTextures.begin();

        while ( iter != m_vTextures.end() )
        {
            CTexture *pTexture = *iter;

            if (pTexture->GetName() == name)
            {
                //m_vTextures.erase( iter );
                delete pTexture;
                break;
            }

            iter++;
        }
    }

    void Insert(String className, CTexture *pObject )
    {
        m_vTextures.push_back(pObject);
    }

    CTexture* Get(String textureName)
    {
        for (unsigned int i = 0; i < m_vTextures.size(); i++)
        {
            CTexture *pTexture = m_vTextures[i];

            if (pTexture != NULL && pTexture->GetName() == textureName)
            {
                return pTexture;
            }
        }

        return NULL;
    }
private:

    //Array m_vTextures;
    vector<CTexture*> m_vTextures;
};

extern CTextureLoader *g_pTextureLoader;

#endif // TEXTURE_H
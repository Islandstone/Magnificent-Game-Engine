#ifndef CAMERA_H
#define CAMERA_H

#include "base.h"
#include "engine.h"

class Camera
{
private:
    D3DXMATRIX  m_pViewMatrix;
    D3DXMATRIX  m_pProjectionMatrix;
    D3DXVECTOR2 m_vecCameraPosition;
    float      m_flZoom;

    float m_flMaxZoom;
    float m_flMinZoom;
    //float m_flRectWidth;
    //float m_flRectHeight;

public:
    Camera(void)
    {
        m_vecCameraPosition = D3DXVECTOR2(0.0f,0.0f);
        m_flZoom = 1.0;
        m_flMaxZoom = 0.0;
        m_flMinZoom = 100.0;
        //m_flRectWidth = ;
        //m_flRectHeight = WINDOW_Y;
    }

    ~Camera(void){}

    inline void SetZoomRange(float min = 100.0, float max = 0.0) { m_flMaxZoom = max; m_flMinZoom = min; }

    inline float GetMaxZoom()  { return m_flMaxZoom; }
    inline float GetMinZoom()  { return m_flMinZoom; }
    inline float GetZoom()     { return m_flZoom;   }

    void Zoom(float zoom)
    {
        float newZoom = m_flZoom * zoom;
        
        if(newZoom > m_flMinZoom)
        {
            m_flZoom = m_flMinZoom;
            return;
        }
        else if(newZoom < m_flMaxZoom)
        {
            m_flZoom = m_flMaxZoom;
            return;
        }

        m_flZoom = newZoom;
    }

    void Update()
    {
        // Setup orthographic projection matrix
        float halfWidth  = Engine()->ScreenWidth() * 0.5f;
        float halfHeight = Engine()->ScreenHeight() * 0.5f;

        D3DXMatrixOrthoOffCenterLH(&this->m_pProjectionMatrix,
            -halfWidth / m_flZoom , halfWidth / m_flZoom,
            halfHeight / m_flZoom, -halfHeight / m_flZoom,
            m_flMaxZoom, m_flMinZoom);

        // Set the camera's view matrix
        D3DXVECTOR3 eye(this->m_vecCameraPosition.x, this->m_vecCameraPosition.y, -m_flZoom );
        D3DXVECTOR3 at(eye.x, eye.y, m_flMaxZoom);

        // Assumes that the up vector is along the y-axis, ie. x is to the right and z is inwards
        D3DXMatrixLookAtLH(&this->m_pViewMatrix, &eye, &at, &D3DXVECTOR3(0.0f,1.0f,0.0f));
        
        Engine()->GetDevice()->SetTransform(D3DTS_PROJECTION, &this->m_pProjectionMatrix);
        Engine()->GetDevice()->SetTransform(D3DTS_VIEW, &this->m_pViewMatrix);
    }

    inline D3DXVECTOR2 GetPosition()   { return m_vecCameraPosition;   }
    inline D3DXMATRIX  GetView()       { return m_pViewMatrix;         }
    inline D3DXMATRIX  GetProjection() { return m_pProjectionMatrix;   }

    //inline void SetRect(float width, float height) { m_flRectWidth = width; m_flRectHeight = height; }

    inline void SetPosition(float x, float y)   { m_vecCameraPosition.x = x; m_vecCameraPosition.y = y; }
    inline void SetPosition(D3DXVECTOR2 position)   { m_vecCameraPosition = position; }

    inline float GetX() { return m_vecCameraPosition.x; }
    inline float GetY() { return m_vecCameraPosition.y; }

    inline void SetX(float new_x) { m_vecCameraPosition.x = new_x; }
    inline void SetY(float new_y) { m_vecCameraPosition.y = new_y; }
};

extern Camera* g_pCamera;

#endif // CAMERA_H
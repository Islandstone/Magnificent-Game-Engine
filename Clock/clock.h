#ifndef CLOCK_H
#define CLOCK_H

#include <d3d9.h>
#include <d3dx9core.h>
#include "smart_ptr.h"
#include "base.h"

class CClock
{
public:

    void Init();

    void Think();
    void Render();

    void Destroy();

protected:

    void DrawCircle(D3DXVECTOR3 *center, const float inner_radius, const float outer_radius, float amount, DWORD color );

private:

    float seconds_percentage;
    float minutes_percentage;
    float hours_percentage;
    float week_percentage;
    float day_percentage;
    float month_percentage;
    CComPtr<IDirect3DVertexBuffer9> v_buffer;
    CComPtr<ID3DXFont> m_pFont;

    String m_sTimeString;
    RECT rtText;
};

#endif // CLOCK_H
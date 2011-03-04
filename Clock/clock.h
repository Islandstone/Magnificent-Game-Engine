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

    void DrawCircle(D3DXVECTOR3 *center, const float inner_radius, const float outer_radius, float amount, bool reversed, DWORD color );

private:

    float m_flSecondsPercent;
    float m_flMinutesPercent;
    float m_flHoursPercent;
    float m_flWeekPercent;
    float m_flDayPercent;
    float m_flMonthPercent;
    
    String m_sTimeString;
    RECT m_rtText;

    CComPtr<IDirect3DVertexBuffer9> m_vertexBuffer;

    CComPtr<ID3DXFont> m_pFont;
    CComPtr<ID3DXLine> m_pLine;
};

#endif // CLOCK_H
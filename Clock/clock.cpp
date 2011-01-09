#include "clock.h"
#include "base.h"
#include "engine.h"
#include <locale.h>
#include <time.h>
#include <algorithm>

struct CUSTOMVERTEX {float x, y, z; DWORD color;};
#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)

#define SEGMENTS 100
#define BUFFER_SIZE 4*SEGMENTS

void CClock::Init(void)
{
    // create a vertex buffer interface called v_buffer
    Engine()->GetDevice()->CreateVertexBuffer(BUFFER_SIZE*sizeof(CUSTOMVERTEX),
        0,
        CUSTOMFVF,
        D3DPOOL_MANAGED,
        &v_buffer,
        NULL);

    // Titillium or 7px2bus
#if 0
    std::locale::global(std::locale(""));

    D3DXCreateFont(Engine()->GetDevice(),     //D3D Device
    72,               //Font height
    0,                //Font width
    FW_NORMAL,        //Font Weight
    1,                //MipLevels
    false,            //Italic
    DEFAULT_CHARSET,  //CharSet
    OUT_DEFAULT_PRECIS, //OutputPrecision
    ANTIALIASED_QUALITY, //Quality
    DEFAULT_PITCH|FF_DONTCARE,//PitchAndFamily
    L"TitilliumText25L-400wt",          //pFacename,
    &m_pFont);         //ppFont

#else
    D3DXCreateFont(Engine()->GetDevice(),     //D3D Device
        60,               //Font height
        0,                //Font width
        FW_NORMAL,        //Font Weight
        1,                //MipLevels
        false,            //Italic
        DEFAULT_CHARSET,  //CharSet
        OUT_DEFAULT_PRECIS, //OutputPrecision
        ANTIALIASED_QUALITY, //Quality
        DEFAULT_PITCH|FF_DONTCARE,//PitchAndFamily
        L"7px2bus",          //pFacename,
        &m_pFont);         //ppFont
#endif

    Engine()->AddObject(m_pFont);

    
    //OutputDebugStringA( std::locale().name().c_str() );
}

void CClock::Destroy()
{
    Engine()->GetDevice()->SetFVF(NULL);
    v_buffer->Release();    // close and release the vertex buffer
}

tm TimeFromSystemTime(const SYSTEMTIME * pTime)
{
    struct tm tm;
    memset(&tm, 0, sizeof(tm));

    tm.tm_year = pTime->wYear - 1900;
    tm.tm_mon = pTime->wMonth - 1;
    tm.tm_mday = pTime->wDay;

    tm.tm_hour = pTime->wHour;
    tm.tm_min = pTime->wMinute;
    tm.tm_sec = pTime->wSecond;

    return tm;
}

void CClock::Think()
{
    SYSTEMTIME systime;
    GetLocalTime( &systime );

    seconds_percentage = (systime.wSecond + (0.001f * systime.wMilliseconds))/60.0f;
    minutes_percentage = systime.wMinute / 60.0f;
    hours_percentage = (systime.wHour) / 24.0f;
    week_percentage = (systime.wDayOfWeek == 0 ? 6 : systime.wDayOfWeek ) / 6.0f;

    int days_in_current_month = 30;

    if (systime.wMonth >= 7)
    {
        if (systime.wMonth == 2)
        {
            days_in_current_month = 28;
        }
        else
        {
            days_in_current_month = systime.wMonth % 2 == 0 ? 31 : 30;
        }
    }
    else
    {
        days_in_current_month = systime.wMonth % 2 == 0 ? 30 : 31;
    }

    day_percentage = systime.wDay / (float)days_in_current_month;

    month_percentage = systime.wMonth / 12.0f;

    wchar_t *pTemp = new wchar_t[256];

    _wcsftime_l(pTemp, 256, L"%H:%M:%S\n%A %d. %B %Y", &TimeFromSystemTime(&systime), _get_current_locale() );
    m_sTimeString = pTemp;
    delete pTemp;

    SetRect(&rtText, 0, (int)(Engine()->ScreenHeight() * 0.75f), (int)Engine()->ScreenWidth(), (int)(Engine()->ScreenHeight()) );
}


void CClock::DrawCircle( D3DXVECTOR3 *center, const float inner_radius, const float outer_radius, float amount, DWORD color )
{
    // First set the transform
    if (center != NULL)
    {
        D3DXMATRIX mat;
        D3DXMatrixTranslation(&mat, center->x, center->y, center->z);

        Engine()->GetDevice()->SetTransform(D3DTS_WORLD, &mat);
    }
    

    CUSTOMVERTEX* vertices;

    // lock v_buffer and load the vertices into it
    v_buffer->Lock(0, 0, (void**)&vertices, 0);

    float wedge_angle = amount * 360.0f;

    int num = 0;

    // Calculate the number of segments on the arc
    int segments = (int)(SEGMENTS * (wedge_angle/360.0f));

    // Must have at least one
    if (segments == 0)
        segments = 1;

    for (int i = 0; i <= segments; i++)
    {
        float angle = wedge_angle * ((float)i/(float)segments);

        D3DXVECTOR3 wedge( cos( D3DXToRadian( angle+270.0f ) ), sin( D3DXToRadian( angle+270.0f ) ), 0.0f);

        num++;
        vertices[num].x = wedge.x * outer_radius;
        vertices[num].y = wedge.y * outer_radius;
        vertices[num].z = 0.5f;
        vertices[num].color = color;

        num++;
        vertices[num].x = wedge.x * inner_radius;
        vertices[num].y = wedge.y * inner_radius;
        vertices[num].z = 0.5f;
        vertices[num].color = color;
    }

    v_buffer->Unlock();

    Engine()->GetDevice()->SetFVF(CUSTOMFVF);
    Engine()->GetDevice()->SetStreamSource(0, v_buffer, 0, sizeof(CUSTOMVERTEX));
    Engine()->GetDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, num-2 + 1); // +1 because num is 0 based
}


void CClock::Render()
{
#define NUMBER_OF_RINGS 6
    float amounts[NUMBER_OF_RINGS] = {
        seconds_percentage,
        minutes_percentage,
        hours_percentage,
        week_percentage,
        day_percentage,
        month_percentage
    };

    static DWORD color[] = {
        D3DCOLOR_XRGB(255, 0, 0),
        D3DCOLOR_XRGB(0, 255,0),
        D3DCOLOR_XRGB(255, 255,0),
        D3DCOLOR_XRGB(0, 0,255),
        D3DCOLOR_XRGB(255, 0,255),
        D3DCOLOR_XRGB(255, 255,255),
    };

    float radius = Engine()->ScreenHeight() * (1.0f/3.0f);
    float spacing = radius * 0.05f;
    float width = radius * 0.1f;

    D3DXVECTOR3 center(0.0f, -radius * 0.35f, 0.0f);

    for (int i = 0; i < NUMBER_OF_RINGS; i++)
    {
        float inner = radius - ( i*( spacing+width ) );
        float outer = radius - ( width + i*( spacing+width ));

        DrawCircle(&center, inner, outer, amounts[i], color[i] );
    }

    if (m_pFont)
    {
        m_pFont->DrawText(NULL, m_sTimeString.c_str(), m_sTimeString.size(), &rtText, DT_CENTER, 0xffffffff );
    }
}
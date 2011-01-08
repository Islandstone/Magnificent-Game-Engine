#include "base.h"
#include "timer.h"

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

CTimer _globaltimer;
CTimer *g_pGlobalTimer = &_globaltimer;

CTimer::CTimer(bool locked)
{
    m_bLocked = locked;
    m_iStart = 0;
    m_iFreq = -1;
}

__int64 CTimer::GetPerformanceCount()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return li.QuadPart;
}

__int64 CTimer::GetFrequency()
{
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
    return li.QuadPart;
}

void CTimer::Start()
{
    m_iStart = CTimer::GetPerformanceCount();
    m_iFreq = CTimer::GetFrequency();
}

float CTimer::CurrentTime()
{
    if (m_iFreq == -1)
    {
        return 0;
    }

    return (float)(CTimer::GetPerformanceCount() - m_iStart)/(float)m_iFreq;
}

void CTimer::Reset()
{
    if (m_bLocked)
    {
        return;
    }

    m_iStart = 0;
    m_iFreq = -1;
}

CFadeTimer::CFadeTimer(float total_duration /* = 5.0f */, float fade /* = 1.5f */) : CTimer(false)
{
    m_flDuration = total_duration;

    if ( 2.0f*fade > m_flDuration )
    {
        m_flFade = 0.5f * m_flDuration;
    }
    else
    {
        m_flFade = fade;
    }

    m_flEndTime = 0.0f;
}

void CFadeTimer::Start()
{
    CTimer::Start();

    m_flFadeStart = CurrentTime();
    m_flFadeEnd = m_flFadeStart + m_flFade;

    m_bSecondStage = false;
}

float CFadeTimer::GetModifier()
{
    float current_time = CurrentTime();

    if (m_bSecondStage)
    {
        // Still at the peak
        if (current_time <= m_flFadeStart)
        {
            return 1.0f;
        }

        // Fade out 
        float retval = 1.0f-(current_time - m_flFadeStart)/m_flFade;

        if (retval > 1.0f)
        {
            return 1.0f;
        }
        else if (retval < 0.0f)
        {
            return 0.0f;
        }

        return retval;
    }
    else /*if (current_time <= m_flFadeEnd)*/
    {
        // Fade in
        float retval = (current_time - m_flFadeStart)/m_flFade;

        if (retval >= 1.0f)
        {
            // Transition from fading in to fade out
            m_bSecondStage = true;
            m_flFadeStart = m_flDuration - m_flFade;

            return 1.0f;
        }

        return retval;
    }

    return 1.0f;
}
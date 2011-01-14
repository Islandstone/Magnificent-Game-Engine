#ifndef TIMER_H
#define TIMER_H

class CTimer
{
public:
    CTimer(bool locked = false);

    // This is only a singleton for the global timer.
    // User code may create CTimer objects as they wish if neccessary
    static CTimer* GlobalTimer()
    {
        static CTimer globalTimer;
        return &globalTimer;
    }

    virtual void Start();
    float CurrentTime();
    virtual void Reset();

    static __int64 GetFrequency();
    static __int64 GetPerformanceCount();

    bool IsStarted() { return m_iFreq != -1; }

protected:

    __int64 m_iStart;
    __int64 m_iFreq;

    bool m_bStarted;

private:

    bool m_bLocked;
};

extern CTimer *g_pGlobalTimer;

class CFadeTimer : public CTimer
{
public:

    CFadeTimer(float total_duration = 5.0f, float fade = 1.5f);
    
    void Start();
    float GetModifier();

    bool IsFinished() { return CurrentTime() >= m_flDuration; }

protected:
private:

    float m_flFadeStart;
    float m_flFadeEnd;
    float m_flDuration;
    float m_flFade;

    float m_flEndTime;
    bool m_bSecondStage;
};

#endif // TIMER_H
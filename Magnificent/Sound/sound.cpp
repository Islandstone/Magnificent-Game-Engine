#include "base.h"
#include "sound.h"
#include "engine.h"
#include "fmod.hpp"
#include "input.h"

#pragma deprecated( g_pSound )
CSoundSystem *g_pSound = NULL;

using namespace FMOD;

String FMOD_ErrorString(FMOD_RESULT errcode);

#ifdef DEBUG
#define ERRCHECK(x) if (x != FMOD_OK) { Engine()->Error( FMOD_ErrorString(x) ); return false; }
#else
#define ERRCHECK(x) if (x != FMOD_OK) { Engine()->Error( FMOD_ErrorString(x) ); return false; }
#endif

CSoundSystem::CSoundSystem()
{
    g_pSound = this;
    m_pSystem = NULL;

    m_bHasPlayedTest = false;
}

bool CSoundSystem::Init()
{
    /*
    *   Large portions of this code is based on the initialization code
    *   provided with the FMOD installation
    */

    FMOD_RESULT      result;
    unsigned int     version;
    int              numdrivers;
    FMOD_SPEAKERMODE speakermode;
    FMOD_CAPS        caps;
    char             name[256];

    /*
        Create a System object and initialize.
    */
    result = System_Create(&m_pSystem);
    ERRCHECK(result);
    
    result = m_pSystem->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        //printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        
        wchar_t szStr[255];
        wchar_t *pszStr = &szStr[0];
        _snwprintf_s(pszStr, ARRAYSIZE(szStr), sizeof(szStr), L"You are using an old version of FMOD, %08x. This program requires %08x", version, FMOD_VERSION );

        Engine()->Error(szStr);
        return false;
    }
    
    result = m_pSystem->getNumDrivers(&numdrivers);
    ERRCHECK(result);

    if (numdrivers == 0)
    {
        result = m_pSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
        ERRCHECK(result);
    }
    else
    {
        result = m_pSystem->getDriverCaps(0, &caps, 0, 0, &speakermode);
        ERRCHECK(result);

        result = m_pSystem->setSpeakerMode(speakermode);       /* Set the user selected speaker mode. */
        ERRCHECK(result);

        if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
        {                                                   /* You might want to warn the user about this. */

            Engine()->Error(L"Hardware accelerated sound has been disabled.\nTo ensure optimum sound quality, go to Control Panel -> Sound -> Volume(tab) -> Advanced(button) -> Performance(tab)\nand partially or fully enable hardware acceleration");

            result = m_pSystem->setDSPBufferSize(1024, 10);
            ERRCHECK(result);
        }

        result = m_pSystem->getDriverInfo(0, name, 256, 0);
        ERRCHECK(result);

        if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
        {
            result = m_pSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
            ERRCHECK(result);
        }
    }

    result = m_pSystem->init(100, FMOD_INIT_NORMAL, 0);
    
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
    {
        result = m_pSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        ERRCHECK(result);
            
        result = m_pSystem->init(100, FMOD_INIT_NORMAL, 0);/* ... and re-init. */
        ERRCHECK(result);
    }

    return true;
}

void CSoundSystem::Play(const String filename)
{
    CSample* pSample = CreateSample(filename);

    if (pSample != NULL)
    {
        pSample->Play();
    }
}

CSample* CSoundSystem::GetSampleByName(const String filename)
{
    for (unsigned int i = 0; i < m_vSamples.size(); i++)
    {
        if ( m_vSamples[i] != NULL && m_vSamples[i]->GetFilename() == filename )
        {
            return m_vSamples[i];
        }
    }

    return NULL;
}

CSample* CSoundSystem::CreateSample(const String filename)
{
    CSample* pSample = GetSampleByName(filename);

    if (pSample != NULL)
    {
        return pSample;
    }

    pSample = new CSample(filename);

    m_vSamples.push_back(pSample);

    return pSample;
}

void CSoundSystem::Update()
{
    m_pSystem->update();

    if (g_pInput->KeyReleased(VK_HOME) && !m_bHasPlayedTest)
    {
        Play(L"startup.mp3");
        m_bHasPlayedTest = true;
    }
    
    if (g_pInput->KeyHeld(VK_SUBTRACT))
    {
        FMOD_RESULT res;
        float curtime = g_pGlobalTimer->CurrentTime();
        //Engine()->Debug(L"Decreasing volume\n");
        
        FMOD::ChannelGroup *pChannelGroup;
        res = m_pSystem->getMasterChannelGroup(&pChannelGroup);

        if (res != FMOD_OK)
        {
            return;
        }

        float current_volume = 1.0f;
        res = pChannelGroup->getVolume(&current_volume);

        if (res != FMOD_OK)
        {
            return;
        }

        res = pChannelGroup->setVolume( max( current_volume - (0.4f * (curtime-m_flLastUpdateTime)), 0.0f) );

        if (res != FMOD_OK)
        {
            return;
        }
    }
    
    if (g_pInput->KeyHeld(VK_ADD))
    {
        FMOD_RESULT res;
        float curtime = g_pGlobalTimer->CurrentTime();

        FMOD::ChannelGroup *pChannelGroup;
        res = m_pSystem->getMasterChannelGroup(&pChannelGroup);

        if (res != FMOD_OK)
        {
            return;
        }

        float current_volume = 1.0f;
        res = pChannelGroup->getVolume(&current_volume);

        if (res != FMOD_OK)
        {
            return;
        }

        res = pChannelGroup->setVolume( min(current_volume + (0.2f * (curtime-m_flLastUpdateTime)), 1.0f) );

        if (res != FMOD_OK)
        {
            return;
        }
    }

    m_flLastUpdateTime = g_pGlobalTimer->CurrentTime();
}

void CSoundSystem::Destroy()
{
    for (unsigned int i = 0; i < m_vSamples.size(); i++)
    {
        Zap( m_vSamples[i] );
    }

    m_vSamples.clear();

    m_pSystem->release();
}

CSample::CSample(String filename)
{
    m_sFilename = filename;
    m_pSound = NULL;
    m_pChannel = NULL;

    LoadFile();
}

CSample::~CSample()
{
    if (m_pSound != NULL)
    {
        m_pSound->release();
        m_pSound = NULL;
    }
}

void CSample::LoadFile()
{
    FMOD_RESULT result = FMOD_OK;
    int buffer_size = m_sFilename.size() + 1;
    char *pszFilename = (char *)malloc( buffer_size );
    size_t temp;

    wcstombs_s(&temp, pszFilename, buffer_size, m_sFilename.c_str(), buffer_size );

    result = g_pSound->FMODSystem()->createStream(pszFilename, FMOD_DEFAULT | FMOD_CREATESTREAM, NULL, &m_pSound);

    if (result != FMOD_OK)
    {
#ifdef DEBUG
        //String str(L"Failed to load sound file " + m_sFilename);
        //Engine()->Error(str);

        String str = L"Failed to load sound file " + m_sFilename + L", Error code was: " + FMOD_ErrorString(result);
        Engine()->Debug(str);
#endif
    }

    Zap(pszFilename);
}

void CSample::Play(bool paused)
{
    FMOD_RESULT result;

    if (m_pSound == NULL)
    {
        return;
    }

    result = g_pSound->FMODSystem()->playSound(FMOD_CHANNEL_FREE, m_pSound, paused, &m_pChannel);

    if (result != FMOD_OK)
    {
        String str(L"Failed to load sound file " + m_sFilename);
        Engine()->Error(str);

#ifdef DEBUG
        str.append(L", Error code was: " + FMOD_ErrorString(result) );
        Engine()->Debug(String(L"Failed to load sound file ") );
#endif
    }
}

void CSample::Stop()
{
    if (m_pChannel != NULL)
    {
        m_pChannel->stop();
    }
}
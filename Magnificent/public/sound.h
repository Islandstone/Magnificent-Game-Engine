#ifndef SOUND_H
#define SOUND_H

#include "game_systems.h"
#include "base.h"
#include <vector>

#ifdef ENABLE_FMOD
    #include "fmod.hpp"
#endif

using namespace std;

class CSample;

class CSoundSystem : public CGameSystem
{
    CSoundSystem();
public:

    static CSoundSystem* GetInstance()
    {
        static CSoundSystem soundSystem;
        return &soundSystem;
    }

    bool Init();
    void Destroy();

    void Update();

    const String GetName() { return L"CSoundSystem"; }
    
    CSample* GetSampleByName(const String filename);
    CSample* CreateSample(const String filename);

    void Play(String filename);
    void StopSound(String filename) {}

    void StopAllSounds() {}

#ifdef ENABLE_FMOD
    FMOD::System* FMODSystem() { return m_pSystem; }
#endif

protected:
private:

#ifdef ENABLE_FMOD
    FMOD::System    *m_pSystem;
    vector<CSample*> m_vSamples;
#endif


    bool m_bHasPlayedTest;

    float m_flLastUpdateTime;
};

extern inline CSoundSystem* Sound()
{
    return CSoundSystem::GetInstance();
}

//extern CSoundSystem *g_pSound;

class CSample
{
public:
    ~CSample();

    void Play(bool paused = false);
    void Stop();

    String GetFilename() { return m_sFilename; }
    
#ifdef ENABLE_FMOD
    bool IsValid() { return m_pSound != NULL; }
#endif

    bool IsPlaying() { return false; }

private:
    friend CSoundSystem;
    CSample(String filename);

    void    LoadFile();

#ifdef ENABLE_FMOD
    FMOD::Sound  *m_pSound;
    FMOD::Channel *m_pChannel;
#endif

    String  m_sFilename;
};

#ifdef ENABLE_FMOD

static String FMOD_ErrorString(FMOD_RESULT errcode)
{
    switch (errcode)
    {
    case FMOD_ERR_ALREADYLOCKED:          return L"Tried to call lock a second time before unlock was called. ";
    case FMOD_ERR_BADCOMMAND:             return L"Tried to call a function on a data type that does not allow this type of functionality (ie calling Sound::lock on a streaming sound). ";
    case FMOD_ERR_CDDA_DRIVERS:           return L"Neither NTSCSI nor ASPI could be initialised. ";
    case FMOD_ERR_CDDA_INIT:              return L"An error occurred while initialising the CDDA subsystem. ";
    case FMOD_ERR_CDDA_INVALID_DEVICE:    return L"Couldn't find the specified device. ";
    case FMOD_ERR_CDDA_NOAUDIO:           return L"No audio tracks on the specified disc. ";
    case FMOD_ERR_CDDA_NODEVICES:         return L"No CD/DVD devices were found. ";
    case FMOD_ERR_CDDA_NODISC:            return L"No disc present in the specified drive. ";
    case FMOD_ERR_CDDA_READ:              return L"A CDDA read error occurred. ";
    case FMOD_ERR_CHANNEL_ALLOC:          return L"Error trying to allocate a channel. ";
    case FMOD_ERR_CHANNEL_STOLEN:         return L"The specified channel has been reused to play another sound. ";
    case FMOD_ERR_COM:                    return L"A Win32 COM related error occured. COM failed to initialize or a QueryInterface failed meaning a Windows codec or driver was not installed properly. ";
    case FMOD_ERR_DMA:                    return L"DMA Failure.  See debug output for more information. ";
    case FMOD_ERR_DSP_CONNECTION:         return L"DSP connection error.  Connection possibly caused a cyclic dependancy. ";
    case FMOD_ERR_DSP_FORMAT:             return L"DSP Format error.  A DSP unit may have attempted to connect to this network with the wrong format. ";
    case FMOD_ERR_DSP_NOTFOUND:           return L"DSP connection error.  Couldn't find the DSP unit specified. ";
    case FMOD_ERR_DSP_RUNNING:            return L"DSP error.  Cannot perform this operation while the network is in the middle of running.  This will most likely happen if a connection or disconnection is attempted in a DSP callback. ";
    case FMOD_ERR_DSP_TOOMANYCONNECTIONS: return L"DSP connection error.  The unit being connected to or disconnected should only have 1 input or output. ";
    case FMOD_ERR_FILE_BAD:               return L"Error loading file. ";
    case FMOD_ERR_FILE_COULDNOTSEEK:      return L"Couldn't perform seek operation.  This is a limitation of the medium (ie netstreams) or the file format. ";
    case FMOD_ERR_FILE_EOF:               return L"End of file unexpectedly reached while trying to read essential data (truncated data?). ";
    case FMOD_ERR_FILE_NOTFOUND:          return L"File not found. ";
    case FMOD_ERR_FILE_UNWANTED:          return L"Unwanted file access occured.";
    case FMOD_ERR_FORMAT:                 return L"Unsupported file or audio format. ";
    case FMOD_ERR_HTTP:                   return L"A HTTP error occurred. This is a catch-all for HTTP errors not listed elsewhere. ";
    case FMOD_ERR_HTTP_ACCESS:            return L"The specified resource requires authentication or is forbidden. ";
    case FMOD_ERR_HTTP_PROXY_AUTH:        return L"Proxy authentication is required to access the specified resource. ";
    case FMOD_ERR_HTTP_SERVER_ERROR:      return L"A HTTP server error occurred. ";
    case FMOD_ERR_HTTP_TIMEOUT:           return L"The HTTP request timed out. ";
    case FMOD_ERR_INITIALIZATION:         return L"FMOD was not initialized correctly to support this function. ";
    case FMOD_ERR_INITIALIZED:            return L"Cannot call this command after System::init. ";
    case FMOD_ERR_INTERNAL:               return L"An error occured that wasnt supposed to.  Contact support. ";
    case FMOD_ERR_INVALID_HANDLE:         return L"An invalid object handle was used. ";
    case FMOD_ERR_INVALID_PARAM:          return L"An invalid parameter was passed to this function. ";
    case FMOD_ERR_INVALID_SPEAKER:        return L"An invalid speaker was passed to this function based on the current speaker m ";
    case FMOD_ERR_IRX:                    return L"PS2 only.  fmodex.irx failed to initialize.  This is most likely because you forgot to load it. ";
    case FMOD_ERR_MEMORY:                 return L"Not enough memory or resources. ";
    case FMOD_ERR_MEMORY_IOP:             return L"PS2 only.  Not enough memory or resources on PlayStation 2 IOP ram. ";
    case FMOD_ERR_MEMORY_SRAM:            return L"Not enough memory or resources on console sound ram. ";
    case FMOD_ERR_NEEDS2D:                return L"Tried to call a command on a 3d sound when the command was meant for 2d sound. ";
    case FMOD_ERR_NEEDS3D:                return L"Tried to call a command on a 2d sound when the command was meant for 3d sound. ";
    case FMOD_ERR_NEEDSHARDWARE:          return L"Tried to use a feature that requires hardware support.  (ie trying to play a VAG compressed sound in software on PS2). ";
    case FMOD_ERR_NEEDSSOFTWARE:          return L"Tried to use a feature that requires the software engine.  Software engine has either been turned off, or command was executed on a hardware channel which does not support this feature. ";
    case FMOD_ERR_NET_CONNECT:            return L"Couldn't connect to the specified host. ";
    case FMOD_ERR_NET_SOCKET_ERROR:       return L"A socket error occurred.  This is a catch-all for socket-related errors not listed elsewhere. ";
    case FMOD_ERR_NET_URL:                return L"The specified URL couldn't be resolved. ";
    case FMOD_ERR_NOTREADY:               return L"Operation could not be performed because specified sound is not ready. ";
    case FMOD_ERR_OUTPUT_ALLOCATED:       return L"Error initializing output device, but more specifically, the output device is already in use and cannot be reused. ";
    case FMOD_ERR_OUTPUT_CREATEBUFFER:    return L"Error creating hardware sound buffer. ";
    case FMOD_ERR_OUTPUT_DRIVERCALL:      return L"A call to a standard soundcard driver failed, which could possibly mean a bug in the driver or resources were missing or exhausted. ";
    case FMOD_ERR_OUTPUT_FORMAT:          return L"Soundcard does not support the minimum features needed for this soundsystem (16bit stereo output). ";
    case FMOD_ERR_OUTPUT_INIT:            return L"Error initializing output device. ";
    case FMOD_ERR_OUTPUT_NOHARDWARE:      return L"FMOD_HARDWARE was specified but the sound card does not have the resources nescessary to play it. ";
    case FMOD_ERR_OUTPUT_NOSOFTWARE:      return L"Attempted to create a software sound but no software channels were specified in System::init. ";
    case FMOD_ERR_PAN:                    return L"Panning only works with mono or stereo sound sources. ";
    case FMOD_ERR_PLUGIN:                 return L"An unspecified error has been returned from a 3rd party plugin. ";
    case FMOD_ERR_PLUGIN_MISSING:         return L"A requested output, dsp unit type or codec was not available. ";
    case FMOD_ERR_PLUGIN_RESOURCE:        return L"A resource that the plugin requires cannot be found. (ie the DLS file for MIDI playback) ";
    case FMOD_ERR_RECORD:                 return L"An error occured trying to initialize the recording device. ";
    case FMOD_ERR_REVERB_INSTANCE:        return L"Specified Instance in FMOD_REVERB_PROPERTIES couldn't be set. Most likely because another application has locked the EAX4 FX slot. ";
    case FMOD_ERR_SUBSOUNDS:              return L" The error occured because the sound referenced contains subsounds.  (ie you cannot play the parent sound as a static sample, only its subsounds.)";
    case FMOD_ERR_SUBSOUND_ALLOCATED:     return L"This subsound is already being used by another sound, you cannot have more than one parent to a sound.  Null out the other parent's entry first. ";
    case FMOD_ERR_TAGNOTFOUND:            return L"The specified tag could not be found or there are no tags. ";
    case FMOD_ERR_TOOMANYCHANNELS:        return L"The sound created exceeds the allowable input channel count.  This can be increased with System::setMaxInputChannels. ";
    case FMOD_ERR_UNIMPLEMENTED:          return L"Something in FMOD hasn't been implemented when it should be! contact support! ";
    case FMOD_ERR_UNINITIALIZED:          return L"This command failed because System::init or System::setDriver was not called. ";
    case FMOD_ERR_UNSUPPORTED:            return L"A command issued was not supported by this object.  Possibly a plugin without certain callbacks specified. ";
    case FMOD_ERR_UPDATE:                 return L"On PS2, System::update was called twice in a row when System::updateFinished must be called first. ";
    case FMOD_ERR_VERSION:                return L"The version number of this file format is not supported. ";
    case FMOD_OK:                         return L"No errors.";
    default :                             return L"Unknown error.";
    };
}
#endif // ENABLE_FMOD


#endif // SOUND_H
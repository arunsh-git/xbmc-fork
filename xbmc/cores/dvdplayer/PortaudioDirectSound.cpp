#ifdef __APPLE__
/*
* XBoxMediaPlayer
* Copyright (c) 2002 d7o3g4q and RUNTiME
* Portions Copyright (c) by the authors of ffmpeg and xvid
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include <portaudio.h>

#include "stdafx.h"
#include "PortAudioDirectSound.h"
#include "AudioContext.h"
#include "Util.h"

void PortAudioDirectSound::DoWork()
{

}

//////////////////////////////////////////////////////////////////////////////
//
// History:
//   12.14.07   ESF  Created.
//
//////////////////////////////////////////////////////////////////////////////
PortAudioDirectSound::PortAudioDirectSound(IAudioCallback* pCallback, int iChannels, unsigned int uiSamplesPerSec, unsigned int uiBitsPerSample, bool bResample, char* strAudioCodec, bool bIsMusic, bool bPassthrough)
{
  CLog::Log(LOGDEBUG,"PortAudioDirectSound::PortAudioDirectSound - opening device");
  
  if (iChannels == 0)
      iChannels = 2;

  bool bAudioOnAllSpeakers = false;
  
  // This should be in the base class.
  g_audioContext.SetupSpeakerConfig(iChannels, bAudioOnAllSpeakers, bIsMusic);
  g_audioContext.SetActiveDevice(CAudioContext::DIRECTSOUND_DEVICE);

  m_bPause = false;
  m_bCanPause = false;
  m_bIsAllocated = false;
  m_uiChannels = iChannels;
  m_uiSamplesPerSec = uiSamplesPerSec;
  m_uiBitsPerSample = uiBitsPerSample;
  m_bPassthrough = bPassthrough;

  m_nCurrentVolume = g_stSettings.m_nVolumeLevel;
  if (!m_bPassthrough)
     m_amp.SetVolume(m_nCurrentVolume);

  m_dwPacketSize = iChannels*(uiBitsPerSample/8)*512;
  m_dwNumPackets = 16;

  /* Open the device */
  CStdString device, deviceuse;
  if (!m_bPassthrough)
    device = g_guiSettings.GetString("audiooutput.audiodevice");
  else
    device = g_guiSettings.GetString("audiooutput.passthroughdevice");

  printf("Asked to open device: [%s]\n", device.c_str());

  int nErr;

#if 0
  /* if this is first access to audio, global sound config might not be loaded */
  if(!snd_config)
    snd_config_update();

  snd_config_t *config = snd_config;
  deviceuse = device;

  nErr = snd_config_copy(&config, snd_config);
  CHECK_ALSA_RETURN(LOGERROR,"config_copy",nErr);

  if(!m_bPassthrough)
  {
    // TODO - add an option to only downmix if user want's us
    if(iChannels == 6)
      deviceuse = "xbmc_51to2:'" + EscapeDevice(deviceuse) + "'";

    // setup channel mapping to linux default
    if (strstr(strAudioCodec, "AAC"))
    {
      if(iChannels == 6)
        deviceuse = "xbmc_aac51:'" + EscapeDevice(deviceuse) + "'";
      else if(iChannels == 5)
        deviceuse = "xbmc_aac50:'" + EscapeDevice(deviceuse) + "'";
    }
    else if (strstr(strAudioCodec, "DMO") || strstr(strAudioCodec, "FLAC") || strstr(strAudioCodec, "PCM"))
    {
      if(iChannels == 6)
        deviceuse = "xbmc_win51:'" + EscapeDevice(deviceuse) + "'";
      else if(iChannels == 5)
        deviceuse = "xbmc_win50:'" + EscapeDevice(deviceuse) + "'";
    }
    else if (strstr(strAudioCodec, "OggVorbis"))
    {
      if(iChannels == 6)
        deviceuse = "xbmc_ogg51:'" + EscapeDevice(deviceuse) + "'";
      else if(iChannels == 5)
        deviceuse = "xbmc_ogg50:'" + EscapeDevice(deviceuse) + "'";
    }


    if(deviceuse != device)
    {
      snd_input_t* input;
      nErr = snd_input_stdio_open(&input, _P("Q:\\system\\asound.conf").c_str(), "r");
      if(nErr >= 0)
      {
        nErr = snd_config_load(config, input);
        CHECK_ALSA_RETURN(LOGERROR,"config_load", nErr);

        snd_input_close(input);
        CHECK_ALSA_RETURN(LOGERROR,"input_close", nErr);
      }
      else
      {
        CLog::Log(LOGWARNING, "%s - Unable to load alsa configuration \"%s\" for device \"%s\" - %s", __FUNCTION__, _P("Q:\\system\\asound.conf").c_str(), deviceuse.c_str(), snd_strerror(nErr));
        deviceuse = device;
      }
    }
  }

  CLog::Log(LOGDEBUG, "%s - using alsa device %s", __FUNCTION__, deviceuse.c_str());

  nErr = snd_pcm_open_lconf(&m_pPlayHandle, deviceuse.c_str(), SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK, config);
  CHECK_ALSA_RETURN(LOGERROR,"pcm_open_lconf",nErr);

  snd_config_delete(config);

  /* Allocate Hardware Parameters structures and fills it with config space for PCM */
  snd_pcm_hw_params_malloc(&hw_params);

  /* Allocate Software Parameters structures and fills it with config space for PCM */
  snd_pcm_sw_params_malloc(&sw_params);

  nErr = snd_pcm_hw_params_any(m_pPlayHandle, hw_params);
  CHECK_ALSA_RETURN(LOGERROR,"hw_params_any",nErr);

  nErr = snd_pcm_hw_params_set_access(m_pPlayHandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
  CHECK_ALSA_RETURN(LOGERROR,"hw_params_set_access",nErr);

  // always use 16 bit samples
  nErr = snd_pcm_hw_params_set_format(m_pPlayHandle, hw_params, SND_PCM_FORMAT_S16_LE);
  CHECK_ALSA_RETURN(LOGERROR,"hw_params_set_format",nErr);

  nErr = snd_pcm_hw_params_set_rate_near(m_pPlayHandle, hw_params, &m_uiSamplesPerSec, NULL);
  CHECK_ALSA_RETURN(LOGERROR,"hw_params_set_rate",nErr);

  nErr = snd_pcm_hw_params_set_channels(m_pPlayHandle, hw_params, iChannels);
  CHECK_ALSA_RETURN(LOGERROR,"hw_params_set_channels",nErr);

  m_maxFrames = m_dwPacketSize ;
  nErr = snd_pcm_hw_params_set_period_size_near(m_pPlayHandle, hw_params, &m_maxFrames, 0);
  CHECK_ALSA_RETURN(LOGERROR,"hw_params_set_period_size",nErr);

  m_BufferSize = snd_pcm_bytes_to_frames(m_pPlayHandle,m_dwPacketSize * 10); // buffer big enough - but not too big...
  nErr = snd_pcm_hw_params_set_buffer_size_near(m_pPlayHandle, hw_params, &m_BufferSize);
  CHECK_ALSA_RETURN(LOGERROR,"hw_params_set_buffer_size",nErr);

  unsigned int periodDuration = 0;
  nErr = snd_pcm_hw_params_get_period_time(hw_params,&periodDuration, 0);
  CHECK_ALSA(LOGERROR,"hw_params_get_period_time",nErr);

  /* Assign them to the playback handle and free the parameters structure */
  nErr = snd_pcm_hw_params(m_pPlayHandle, hw_params);
  CHECK_ALSA_RETURN(LOGERROR,"snd_pcm_hw_params",nErr);

  nErr = snd_pcm_sw_params_current(m_pPlayHandle, sw_params);
  CHECK_ALSA_RETURN(LOGERROR,"sw_params_current",nErr);

  nErr = snd_pcm_sw_params_set_start_threshold(m_pPlayHandle, sw_params, m_dwPacketSize);
  CHECK_ALSA_RETURN(LOGERROR,"sw_params_set_start_threshold",nErr);

  nErr = snd_pcm_sw_params(m_pPlayHandle, sw_params);
  CHECK_ALSA_RETURN(LOGERROR,"snd_pcm_sw_params",nErr);

  m_bCanPause = !!snd_pcm_hw_params_can_pause(hw_params);

  snd_pcm_hw_params_free (hw_params);
  snd_pcm_sw_params_free (sw_params);  

  nErr = snd_pcm_prepare (m_pPlayHandle);
  CHECK_ALSA(LOGERROR,"snd_pcm_prepare",nErr);

  m_bIsAllocated = true;
#endif
}

//***********************************************************************************************
PortAudioDirectSound::~PortAudioDirectSound()
{
  CLog::Log(LOGDEBUG,"PortAudioDirectSound() dtor\n");
  Deinitialize();
}


//***********************************************************************************************
HRESULT PortAudioDirectSound::Deinitialize()
{
#if 0
  CLog::Log(LOGDEBUG,"PortAudioDirectSound::Deinitialize\n");

  m_bIsAllocated = false;
  if (m_pPlayHandle)
  {
    snd_pcm_drop(m_pPlayHandle);
    snd_pcm_close(m_pPlayHandle);
  }

  m_pPlayHandle=NULL;
  	CLog::Log(LOGDEBUG,"PortAudioDirectSound::Deinitialize - set active\n");
  g_audioContext.SetActiveDevice(CAudioContext::DEFAULT_DEVICE);
#endif
  return S_OK;
}

void PortAudioDirectSound::Flush() {
#if 0
  if (m_pPlayHandle == NULL)
     return;

  int nErr = snd_pcm_drop(m_pPlayHandle);
  CHECK_ALSA(LOGERROR,"flush-drop",nErr);
  nErr = snd_pcm_prepare(m_pPlayHandle);
  CHECK_ALSA(LOGERROR,"flush-prepare",nErr);
  nErr = snd_pcm_start(m_pPlayHandle);
  CHECK_ALSA(LOGERROR,"flush-start",nErr); 
#endif
}

//***********************************************************************************************
HRESULT PortAudioDirectSound::Pause()
{
  if (m_bPause) return S_OK;
  m_bPause = true;

#if 0
  if(m_bCanPause)
  {
    int nErr = snd_pcm_pause(m_pPlayHandle,1); // this is not supported on all devices.     
    CHECK_ALSA(LOGERROR,"pcm_pause",nErr);
    if(nErr<0)
      m_bCanPause = false;
  }
#endif 

  if(!m_bCanPause)
    Flush();

  return S_OK;
}

//***********************************************************************************************
HRESULT PortAudioDirectSound::Resume()
{
#if 0
  // Resume is called not only after Pause but also at certain other points. like after stop when DVDPlayer is flushed.  
  if(m_bCanPause && m_bPause)
    snd_pcm_pause(m_pPlayHandle,0);
#endif

  // If we are not pause, stream might not be prepared to start flush will do this for us
  if(!m_bPause)
    Flush();

  m_bPause = false;

  return S_OK;
}

//***********************************************************************************************
HRESULT PortAudioDirectSound::Stop()
{
#if 0
  if (m_pPlayHandle)
     snd_pcm_drop(m_pPlayHandle);
#endif

  m_bPause = false;

  return S_OK;
}

//***********************************************************************************************
LONG PortAudioDirectSound::GetMinimumVolume() const
{
  return -60;
}

//***********************************************************************************************
LONG PortAudioDirectSound::GetMaximumVolume() const
{
  return 60;
}

//***********************************************************************************************
LONG PortAudioDirectSound::GetCurrentVolume() const
{
  return m_nCurrentVolume;
}

//***********************************************************************************************
void PortAudioDirectSound::Mute(bool bMute)
{
  if (!m_bIsAllocated) return;

  if (bMute)
    SetCurrentVolume(GetMinimumVolume());
  else
    SetCurrentVolume(m_nCurrentVolume);

}

//***********************************************************************************************
HRESULT PortAudioDirectSound::SetCurrentVolume(LONG nVolume)
{
  if (!m_bIsAllocated || m_bPassthrough) return -1;
  m_nCurrentVolume = nVolume;
  m_amp.SetVolume(nVolume);
  return S_OK;
}


//***********************************************************************************************
DWORD PortAudioDirectSound::GetSpace()
{
  if (!m_bIsAllocated) return 0;

#if 0
  int nSpace = snd_pcm_avail_update(m_pPlayHandle);
  if (nSpace < 0) {
     CLog::Log(LOGWARNING,"PortAudioDirectSound::GetSpace - get space failed. err: %d (%s)", nSpace, snd_strerror(nSpace));
     nSpace = 0;
  }

  return nSpace;
#endif
  return 0;
}

//***********************************************************************************************
DWORD PortAudioDirectSound::AddPackets(unsigned char *data, DWORD len)
{
#if 0
  if (!m_pPlayHandle) {
	CLog::Log(LOGERROR,"PortAudioDirectSound::AddPackets - sanity failed. no play handle!");
	return len; 
  }
#endif
  
  // If we are paused we don't accept any data as pause doesn't always
  // work, and then playback would start again
  //
  if(m_bPause)
    return 0;

  DWORD nAvailSpace = GetSpace();

#if 0
  // if there is no room in the buffer - even for one frame, return 
  if ( snd_pcm_frames_to_bytes(m_pPlayHandle,nAvailSpace) < (int) len )
  {
    if (nAvailSpace <= m_maxFrames || (DWORD)snd_pcm_frames_to_bytes(m_pPlayHandle,nAvailSpace) <= m_dwPacketSize)
       return 0;

    len = snd_pcm_frames_to_bytes(m_pPlayHandle,nAvailSpace);
  }

  unsigned char *pcmPtr = data;

  while (pcmPtr < data + (int)len){  
	int nPeriodSize = snd_pcm_frames_to_bytes(m_pPlayHandle,m_maxFrames); // write max frames.
	if ( pcmPtr + nPeriodSize > data + (int)len) {
		nPeriodSize = data + (int)len - pcmPtr;
	}
	
	int framesToWrite = snd_pcm_bytes_to_frames(m_pPlayHandle,nPeriodSize);

	// handle volume de-amp 
	if (!m_bPassthrough)
           m_amp.DeAmplify((short *)pcmPtr, framesToWrite * m_uiChannels);
	
	int writeResult = snd_pcm_writei(m_pPlayHandle, pcmPtr, framesToWrite);
	if (  writeResult == -EPIPE  ) {
		CLog::Log(LOGDEBUG, "PortAudioDirectSound::AddPackets - buffer underun (tried to write %d frames)",
						framesToWrite);
		int err = snd_pcm_prepare(m_pPlayHandle);
		CHECK_ALSA(LOGERROR,"prepare after EPIPE", err);
	}
	else if (writeResult != framesToWrite) {
		CLog::Log(LOGERROR, "PortAudioDirectSound::AddPackets - failed to write %d frames. "
						"bad write (err: %d) - %s",
						framesToWrite, writeResult, snd_strerror(writeResult));
	}

    if (writeResult>0)
		pcmPtr += snd_pcm_frames_to_bytes(m_pPlayHandle,writeResult);
	else
		pcmPtr += snd_pcm_frames_to_bytes(m_pPlayHandle,framesToWrite); 
  }
#endif

  return len;
}

//***********************************************************************************************
FLOAT PortAudioDirectSound::GetDelay()
{
  double delay = 0.0;

#if 0
  double fbps = (double)m_uiSamplesPerSec * 2.0 * (double)m_uiChannels;
  snd_pcm_sframes_t frames = 0;
    
  int nErr = snd_pcm_delay(m_pPlayHandle, &frames);
  CHECK_ALSA(LOGERROR,"snd_pcm_delay",nErr); 
  if (nErr < 0)
     return (double)snd_pcm_frames_to_bytes(m_pPlayHandle,m_BufferSize) / fbps;

  if (frames < 0) {
#if SND_LIB_VERSION >= 0x000901 /* snd_pcm_forward() exists since 0.9.0rc8 */
    snd_pcm_forward(m_pPlayHandle, -frames);
#endif
    frames = 0;
  }

  int nBytes = snd_pcm_frames_to_bytes(m_pPlayHandle,frames);
  delay = (double)nBytes / fbps;

  if (g_audioContext.IsAC3EncoderActive())
    delay += 0.049;
  else
    delay += 0.008;
#endif
  return delay;
}

//***********************************************************************************************
DWORD PortAudioDirectSound::GetChunkLen()
{
  return m_dwPacketSize;
}
//***********************************************************************************************
int PortAudioDirectSound::SetPlaySpeed(int iSpeed)
{
  return 0;
}

void PortAudioDirectSound::RegisterAudioCallback(IAudioCallback *pCallback)
{
  m_pCallback = pCallback;
}

void PortAudioDirectSound::UnRegisterAudioCallback()
{
  m_pCallback = NULL;
}

void PortAudioDirectSound::WaitCompletion()
{

}

void PortAudioDirectSound::SwitchChannels(int iAudioStream, bool bAudioOnAllSpeakers)
{
    return ;
}
#endif
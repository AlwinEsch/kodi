#pragma once
/*
 *      Copyright (C) 2010-2014 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <vector>

#include "ActiveAEDSP.h"

namespace ActiveAE
{
  class CSampleBuffer;
  class CActiveAEResample;

  //@{
  /*!
   * Individual DSP Processing class
   */
  class CActiveAEDSPProcess
  {
    public:
      CActiveAEDSPProcess(AE_DSP_STREAM_ID streamId);
      virtual ~CActiveAEDSPProcess();

      //@{
      /*!>
       * Create the dsp processing with check of all addons about the used input and output audio format.
       * @param inputFormat The used audio stream input format
       * @param outputFormat Audio output format which is needed to send to the sinks
       * @param upmix stereo upmix value from XBMC settings is passed in with it
       * @param quality The requested quality from settings
       * @param streamType The input stream type to find allowed master process dsp addons for it, e.g. AE_DSP_ASTREAM_MUSIC
       * @return True if the dsp processing becomes available
       */
      bool Create(AEAudioFormat inputFormat, AEAudioFormat outputFormat, bool upmix, AEQuality quality, AE_DSP_STREAMTYPE streamType);

      /*!>
       * Destroy all allocated dsp addons for this stream id and stops the processing.
       */
      void Destroy();

      /*!>
       * Force processing function (Process(...)) to perform a reinitialization of addons and data
       */
      void ForceReinit();

      /*!>
       * Get the stream id for this processing class
       */
      AE_DSP_STREAM_ID GetStreamId() const;

      /*!>
       * Get the currently used input stream format
       * @note used to have a fallback to normal operation without dsp
       */
      AEAudioFormat GetInputFormat();

      /*!>
       * Get the incoming channels amount for this class
       */
      unsigned int GetInputChannels();

      /*!>
       * Get the incoming sample rate for this class
       */
      unsigned int GetInputSamplerate();

      /*!>
       * Get the incoming channel names as string
       */
      std::string GetInputChannelNames();

      /*!>
       * Get the inside addons used samplerate for this class
       */
      unsigned int GetProcessSamplerate();

      /*!>
       * Get the outgoing channels amount for this class
       */
      unsigned int GetOutputChannels();

      /*!>
       * Get the outgoing channel names
       */
      std::string GetOutputChannelNames();

      /*!>
       * Get the used output samplerate for this class
       */
      unsigned int GetOutputSamplerate();

      /*!>
       * Get the amount of percent what the cpu need to process complete dsp stream
       * @return The current cpu usage
       */
      float GetCPUUsage(void) const;

      /*!>
       * Get the channel layout which is passed out from it
       * @return Channel information class
       */
      CAEChannelInfo GetChannelLayout();

      /*!>
       * Get the currently used output data fromat
       * @note Is normally float
       */
      AEDataFormat GetDataFormat();

      /*!>
       * It returns the on input source detected stream type, not always the active one.
       */
      AE_DSP_STREAMTYPE GetDetectedStreamType();

      /*!>
       * Get the currently on addons processed audio stream type which is set from XBMC,
       * it is user selectable or if auto mode is enabled it becomes detected upon the
       * stream input source, eg. Movie, Music...
       */
      AE_DSP_STREAMTYPE GetUsedStreamType();

      /*!>
       * Get the currently on addons processed audio base type which is detected from XBMC.
       * The base type is relevant to the type of input source, eg. Mono, Stereo, Dolby Digital...
       */
      AE_DSP_BASETYPE GetUsedBaseType();

      /*!>
       * Used to check for active modes to given type on running stream id
       * @param type The type to get, AE_DSP_MODE_TYPE_UNDEFINED to get from all types
       * @return true if any of selected type is present
       */
      bool HasActiveModes(AE_DSP_MODE_TYPE type);

      /*!>
       * Used to get all active mode classes to given type on running stream id
       * @param type The type to get, AE_DSP_MODE_TYPE_UNDEFINED to get from all types
       * @retval modes The active modes classes in process chain list
       */
      void GetActiveModes(AE_DSP_MODE_TYPE type, std::vector<CActiveAEDSPModePtr> &modes);

      /*!>
       * Used to get all available Master modes on current stream and base type.
       * It is used to get informations about selectable modes and can be used as information
       * for the gui to make the mode selection available.
       * @param streamType the stream type to check about available master modes, e.g. AE_DSP_ASTREAM_MUSIC
       * @retval modes Pointer to a buffer array where all available master mode written in
       */
      void GetAvailableMasterModes(AE_DSP_STREAMTYPE streamType, std::vector<CActiveAEDSPModePtr> &modes);

      /*!>
       * Get the 32 bit individual identification code of the running master mode
       * @return The identification code, or 0 if no master process is running
       */
      int GetActiveMasterModeID();

      /*!>
       * Returns the information class from the currently used dsp addon
       * @return pointer to the info class, or unitialized class if no master processing present
       */
      CActiveAEDSPModePtr GetActiveMasterMode() const;

      /*!>
       * Change master mode with the 32 bit individual identification code, the change
       * is not directly performed in this function, is changed on next processing
       * calls and must be observed that it becomes changed.
       * @param streamType Stream type on the selection of the master mode
       * @param iModeID The database identification code of the mode
       * @param bSwitchStreamType force switching to given stream type
       * @return True if the mode is allowed and can become changed
       */
      bool SetMasterMode(AE_DSP_STREAMTYPE streamType, int iModeID, bool bSwitchStreamType = false);

      /*!>
       * Read a description string from currently processed audio dsp master mode.
       * It can be used to show additional stream information as string on the skin.
       * The addon can have more stream information.
       * @retval strInfo Pointer to a string where it is written in
       * @return Returns false if no master processing is enabled
       */
      bool GetMasterModeStreamInfoString(CStdString &strInfo);

      /*!>
       * Get all dsp addon relavant information to detect processing mode type and base values.
       * @retval streamTypeUsed The current stream type processed by addon
       * @retval baseType The current base type type processed by addon
       * @retval iModeID The database identification code of the mode
       * @return Returns false if no master processing is enabled
       */
      bool GetMasterModeTypeInformation(AE_DSP_STREAMTYPE &streamTypeUsed, AE_DSP_BASETYPE &baseType, int &iModeID);

      /*!>
       * Used check that asked addon with his mode id is used on given stream identifier
       * @param category The type to get
       * @param iAddonId The ID of the addon to get the menu entries for it
       * @param iModeNumber From addon defined identifier of the mode
       * @return true if in use
       */
      bool IsMenuHookModeActive(AE_DSP_MENUHOOK_CAT category, int iAddonId, unsigned int iModeNumber);

    protected:
      friend class CActiveAEBufferPoolResample;

      /*!>
       * Master processing
       * @param in the ActiveAE input samples
       * @param out the processed ActiveAE output samples
       * @return true if processing becomes performed correct
       */
      bool Process(CSampleBuffer *in, CSampleBuffer *out);

      /*!>
       * Returns the time in seconds that it will take
       * for the next added packet to be heard from the speakers.
       * @return seconds
       */
      float GetDelay();
    //@}
    private:
    //@{
      /*!
       * Helper functions
       */
      void InitFFMpegDSPProcessor();
      bool CreateStreamProfile();
      void ResetStreamFunctionsSelection();
      AE_DSP_STREAMTYPE DetectStreamType(const CFileItem *item);
      const char *GetStreamTypeName(AE_DSP_STREAMTYPE iStreamType);
      void ClearArray(float **array, unsigned int samples);
      bool MasterModeChange(int iModeID, AE_DSP_STREAMTYPE iStreamType = AE_DSP_ASTREAM_INVALID);
      AE_DSP_BASETYPE GetBaseType(AE_DSP_STREAM_PROPERTIES *props);
      bool ReallocProcessArray(unsigned int requestSize);
      void SetFFMpegDSPProcessorArray(float **array_in, float **array_out);
    //@}
    //@{
      /*!
       * Data
       */
      const AE_DSP_STREAM_ID            m_StreamId;                 /*!< stream id of this class, is a increase/decrease number of the amount of process streams */
      AE_DSP_STREAMTYPE                 m_StreamTypeDetected;       /*! The detected stream type of the stream from the source of it */
      AE_DSP_STREAMTYPE                 m_StreamTypeUsed;           /*!< The currently used stream type */
      bool                              m_ForceInit;                /*!< if set to true the process function perform a reinitialization of addons and data */
      bool                              m_StereoUpmix;              /*!< true if it was enabled by settings */
      AE_DSP_ADDONMAP                   m_usedMap;                  /*!< a map of all currently used audio dsp add-on's */
      AEAudioFormat                     m_InputFormat;              /*!< the used input stream format */
      AEAudioFormat                     m_OutputFormat;             /*!< the from XBMX requested output format */
      unsigned int                      m_OutputSamplerate;         /*!< the currently active output samplerate can be become changed from addon resamplers */
      AEQuality                         m_StreamQuality;            /*!< from XBMC requested stream quality, based also to addons */
      enum AEDataFormat                 m_dataFormat;               /*!< The inside addon system used data format, currently fixed to float */
      AE_DSP_SETTINGS                   m_AddonSettings;            /*!< the current stream's settings passed to dsp add-ons */
      AE_DSP_STREAM_PROPERTIES          m_AddonStreamProperties;    /*!< the current stream's properties (eg. stream type) passed to dsp add-ons */
      int                               m_NewMasterMode;            /*!< if master mode is changed it set here and handled by process function */
      AE_DSP_STREAMTYPE                 m_NewStreamType;            /*!< if stream type is changed it set here and handled by process function */

      CCriticalSection                  m_critSection;
      CCriticalSection                  m_restartSection;

      /*!>
       * Selected dsp addon functions
       */
      struct sDSPProcessHandle
      {
        void Clear()
        {
          iAddonModeNumber = -1;
          iLastTime        = 0;
          iLastUsage       = 0;
        }
        unsigned int        iAddonModeNumber;                       /*!< The identifier, send from addon during mode registration and can be used from addon to select mode from a function table */
        CActiveAEDSPModePtr pMode;                                  /*!< Processing mode information data */
        AE_DSP_ADDON        pAddon;                                 /*!< Addon control class */
        uint64_t            iLastUsage;                             /*!< the last usage time stamp of the mode */
        uint64_t            iLastTime;                              /*!< last processing time of the mode */
      };
      std::vector <AE_DSP_ADDON>        m_Addons_InputProc;         /*!< Input processing list, called to all enabled dsp addons with the basic unchanged input stream, is read only. */
      sDSPProcessHandle                 m_Addon_InputResample;      /*!< Input stream resampling over one on settings enabled input resample function only on one addon */
      std::vector <sDSPProcessHandle>   m_Addons_PreProc;           /*!< Input stream preprocessing function calls set and aligned from dsp settings stored inside database */
      std::vector <sDSPProcessHandle>   m_Addons_MasterProc;        /*!< The current from user selected master processing function on addon */
      int                               m_ActiveMode;               /*!< the current used master mode, is a pointer to m_Addons_MasterProc */
      int                               m_ActiveModeOutChannels;    /*!< Amount of channels given from active master mode or -1 if unhandled */
      unsigned long                     m_ActiveModeOutChannelsPresent; /*! The exact present flags of output processing channels from active master mode */
      std::vector <sDSPProcessHandle>   m_Addons_PostProc;          /*!< Output stream postprocessing function calls set and aligned from dsp settings stored inside database */
      sDSPProcessHandle                 m_Addon_OutputResample;     /*!< Output stream resampling over one on settings enabled output resample function only on one addon */

      /*!>
       * Process arrays
       */
      float                            *m_ProcessArray[2][AE_DSP_CH_MAX];
      unsigned int                      m_ProcessArraySize;

      /*!>
       * CPU usage data
       */
      uint64_t                          m_iLastProcessTime;
      uint64_t                          m_iLastProcessUsage;
      float                             m_fLastProcessUsage;

      /*!>
       * Internal ffmpeg process data
       */
      #define FFMPEG_PROC_ARRAY_IN  0
      #define FFMPEG_PROC_ARRAY_OUT 1
      CActiveAEResample                *m_FFMpegDSPProcessor;       /*!< ffmpeg resampler usage for down mix of input stream to required output channel alignment or internal processing*/
      float                            *m_FFMpegProcessArray[2][AE_DSP_CH_MAX]; /*!< the process array memory pointers for ffmpeg. No own memory only addresses taken from m_ProcessArray in correct ffmpeg channel alignment */

      /*!>
       * Index pointers for interleaved audio streams to detect correct channel alignment
       */
      int                               m_idx_in[AE_CH_MAX];
      uint64_t                          m_ChannelLayoutIn;
      int                               m_idx_out[AE_CH_MAX];
      uint64_t                          m_ChannelLayoutOut;
    //@}
  };
  //@}
}

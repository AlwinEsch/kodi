/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "DVDInputStream.h"
#include "IVideoPlayer.h"
#include "addons/AddonProvider.h"
#include "addons/interface/InstanceHandler.h"
#include "addons/kodi-dev-kit/include/kodi/c-api/addon-instance/inputstream.h"

#include <memory>
#include <vector>

class CInputStreamProvider
  : public ADDON::IAddonProvider
{
public:
  CInputStreamProvider(const ADDON::AddonInfoPtr& addonInfo, KODI_HANDLE parentInstance);

  void GetAddonInstance(INSTANCE_TYPE instance_type,
                        ADDON::AddonInfoPtr& addonInfo,
                        KODI_HANDLE& parentInstance) override;

private:
  ADDON::AddonInfoPtr m_addonInfo;
  KODI_HANDLE m_parentInstance;
};

//! \brief Input stream class
class CInputStreamAddon : public KODI::ADDONS::INTERFACE::IInstanceHandler,
                          public CDVDInputStream,
                          public CDVDInputStream::IDisplayTime,
                          public CDVDInputStream::ITimes,
                          public CDVDInputStream::IPosTime,
                          public CDVDInputStream::IDemux,
                          public CDVDInputStream::IChapter
{
public:
  CInputStreamAddon(const ADDON::AddonInfoPtr& addonInfo,
                    IVideoPlayer* player,
                    const CFileItem& fileitem,
                    const std::string& instanceId);
  ~CInputStreamAddon() override;

  static bool Supports(const ADDON::AddonInfoPtr& addonInfo, const CFileItem& fileitem);

  // CDVDInputStream
  bool Open() override;
  void Close() override;
  int Read(uint8_t* buf, int buf_size) override;
  int64_t Seek(int64_t offset, int whence) override;
  int64_t GetLength() override;
  int GetBlockSize() override;
  bool IsEOF() override;
  bool CanSeek() override; //! @todo drop this
  bool CanPause() override;

  // IDisplayTime
  CDVDInputStream::IDisplayTime* GetIDisplayTime() override;
  int GetTotalTime() override;
  int GetTime() override;

  // ITime
  CDVDInputStream::ITimes* GetITimes() override;
  bool GetTimes(Times &times) override;

  // IPosTime
  CDVDInputStream::IPosTime* GetIPosTime() override;
  bool PosTime(int ms) override;

  // IDemux
  CDVDInputStream::IDemux* GetIDemux() override;
  bool OpenDemux() override;
  DemuxPacket* ReadDemux() override;
  CDemuxStream* GetStream(int streamId) const override;
  std::vector<CDemuxStream*> GetStreams() const override;
  void EnableStream(int streamId, bool enable) override;
  bool OpenStream(int streamid) override;

  int GetNrOfStreams() const override;
  void SetSpeed(int speed) override;
  bool SeekTime(double time, bool backward = false, double* startpts = nullptr) override;
  void AbortDemux() override;
  void FlushDemux() override;
  void SetVideoResolution(int width, int height) override;
  bool IsRealtime() override;

  // IChapter
  CDVDInputStream::IChapter* GetIChapter() override;
  int GetChapter() override;
  int GetChapterCount() override;
  void GetChapterName(std::string& name, int ch = -1) override;
  int64_t GetChapterPos(int ch = -1) override;
  bool SeekChapter(int ch) override;

  /*!
   * Callbacks from add-on to kodi
   */
  //@{
  /*!
   * @brief Allocate a demux packet. Free with FreeDemuxPacket
   * @param kodiInstance A pointer to the add-on.
   * @param iDataSize The size of the data that will go into the packet
   * @return The allocated packet.
   */
  DEMUX_PACKET* cb_allocate_demux_packet(int iDataSize = 0);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_inputstream_allocate_demux_packet>---*/

  /*!
  * @brief Allocate an encrypted demux packet. Free with FreeDemuxPacket
  * @param kodiInstance A pointer to the add-on.
  * @param dataSize The size of the data that will go into the packet
  * @param encryptedSubsampleCount The number of subsample description blocks to allocate
  * @return The allocated packet.
  */
  DEMUX_PACKET* cb_allocate_encrypted_demux_packet(unsigned int dataSize,
                                                   unsigned int encryptedSubsampleCount);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_inputstream_allocate_encrypted_demux_packet>---*/

  /*!
   * @brief Free a packet that was allocated with AllocateDemuxPacket
   * @param kodiInstance A pointer to the add-on.
   * @param pPacket The packet to free.
   */
  void cb_free_demux_packet(DEMUX_PACKET* pPacket);
  /*---AUTO_GEN_PARSE<CB:kodi_addon_inputstream_free_demux_packet>---*/

  /*!
   * @brief Callback used by @ref GetStream to get the data
   *
   * Used as callback to prevent memleaks as the temporary stack memory on addon
   * can be used to give data to Kodi.
   *
   * @param[in] handle Pointer to identify this class
   * @param[in] streamId The related stream Identifier
   * @param[in] stream "C" structure with stream information
   * @return The created demux stream packet
   */
  static KODI_HANDLE cb_get_stream_transfer(KODI_HANDLE handle,
                                            int streamId,
                                            INPUTSTREAM_INFO* stream);
  //@}

protected:
  static int ConvertVideoCodecProfile(STREAMCODEC_PROFILE profile);

  IVideoPlayer* m_player;

private:
  std::vector<std::string> m_fileItemProps;
  INPUTSTREAM_CAPABILITIES m_caps;

  int m_streamCount = 0;

  std::shared_ptr<CInputStreamProvider> m_subAddonProvider;
};

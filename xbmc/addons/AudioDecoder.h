#pragma once
/*
 *      Copyright (C) 2013 Arne Morten Kvarving
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

#include "AddonDll.h"
#include "addons/kodi-addon-dev-kit/include/kodi/kodi_audiodec_types.h"
#include "cores/paplayer/ICodec.h"
#include "music/tags/ImusicInfoTagLoader.h"
#include "filesystem/MusicFileDirectory.h"

namespace MUSIC_INFO
{
  class CMusicInfoTag;
  class EmbeddedArt;
}

namespace ADDON
{

  class CAudioDecoder : public CAddonDll,
                        public ICodec,
                        public MUSIC_INFO::IMusicInfoTagLoader,
                        public XFILE::CMusicFileDirectory
  {
  public:
    static std::unique_ptr<CAudioDecoder> FromExtension(AddonProps props, const cp_extension_t* ext);

    explicit CAudioDecoder(AddonProps props);
    CAudioDecoder(AddonProps props, std::string extension,
                  std::string mimetype, bool tags,
                  bool tracks, std::string codecName,
                  std::string strExt);

    virtual ~CAudioDecoder();

    // Child functions related to ICodec
    virtual bool Init(const CFileItem& file, unsigned int filecache) override;
    virtual int ReadPCM(BYTE* buffer, int size, int* actualsize) override;
    virtual bool Seek(int64_t time) override;
    virtual bool CanInit() override { return true; }

    // Child functions related to CMusicFileDirectory
    virtual int GetTrackCount(const std::string& strPath) override;
    
    void Destroy();
    bool Load(const std::string& strFileName,
              MUSIC_INFO::CMusicInfoTag& tag,
              MUSIC_INFO::EmbeddedArt *art = nullptr);

    const std::string& GetExtensions() const { return m_extension; }
    const std::string& GetMimetypes() const { return m_mimetype; }
    bool HasTags() const { return m_tags; }
    bool HasTracks() const { return m_tracks; }

  protected:
    std::string m_extension;
    std::string m_mimetype;
    bool m_tags;
    bool m_tracks;
    AEChannel m_channel[AE_CH_MAX];

  private:
    void DeInit();
    void ExceptionHandle(std::exception& ex, const char* function);

    sAddonInstance_AudioDecoder m_struct;
    void* m_addonInstance;
  };

} /* namespace ADDON */

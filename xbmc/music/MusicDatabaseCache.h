#pragma once
/*
 *      Copyright (C) 2017 Team XBMC
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

#include <map>
#include <memory>
#include <threads/Thread.h>

#include "MusicDatabase.h"

class CFileItem;
class CAlbum;

namespace MUSIC_INFO
{
  class CMusicInfoTag;
}

namespace MUSICDBCACHE
{
  template <typename T>
  class CMusicDatabaseCacheItem
  {
  public:
    CMusicDatabaseCacheItem()
    {
      m_getDetails = 0;
    }

    int m_getDetails;
    std::shared_ptr<T> m_item;
  };

  class CMusicDatabaseTranslationItem
  {
  public:
    CMusicDatabaseTranslationItem()
    {
      m_updatedAt = 0;
    }

    uint64_t m_updatedAt;
    std::string m_language;
    std::string m_text;
  };

  typedef std::map<long, CMusicDatabaseCacheItem<CFileItem> > tFileItemCacheMap;
  typedef std::map<long, CMusicDatabaseCacheItem<MUSIC_INFO::CMusicInfoTag> > tMusicInfoTagCacheMap;

  typedef std::map<std::string, std::string> tArtTypeCacheType;
  typedef std::map<long, CMusicDatabaseCacheItem<tArtTypeCacheType> > tArtCacheMap;
  typedef std::map<std::string, tArtCacheMap> tArtTypeCacheMap;

  typedef std::pair<std::string, std::string> tArtistArtTypeCacheType;
  typedef std::map<long, CMusicDatabaseCacheItem<tArtistArtTypeCacheType> > tArtistArtCacheMap;
  typedef std::map<std::string, tArtistArtCacheMap> tArtistArtTypeCacheMap;

  typedef std::map<int, std::shared_ptr<std::vector<ArtForThumbLoader> > > tArtThumbLoaderType;
  typedef std::map<bool,tArtThumbLoaderType>   tArtThumbLoaderType_a;
  typedef std::map<int, tArtThumbLoaderType_a> tArtThumbLoaderType_b;
  typedef std::map<int, tArtThumbLoaderType_b> tArtThumbLoaderType_c;
  typedef std::map<int, tArtThumbLoaderType_c> tArtThumbLoaderTypeMap;

  typedef std::map<std::string, CMusicDatabaseTranslationItem> tTranslationCacheMap;

  class CMusicDatabaseCache
  {
  public:
    CMusicDatabaseCache();
    virtual ~CMusicDatabaseCache();

    void addSong(long id, std::shared_ptr<MUSIC_INFO::CMusicInfoTag>& item, int getDetails);
    std::shared_ptr<MUSIC_INFO::CMusicInfoTag> getSong(long id, int getDetails);

    void addAlbum(long id, std::shared_ptr<MUSIC_INFO::CMusicInfoTag>& item, int getDetails);
    std::shared_ptr<MUSIC_INFO::CMusicInfoTag> getAlbum(long id, int getDetails);

    void addArtMap(long id, std::shared_ptr<tArtTypeCacheType>& item, std::string type);
    std::shared_ptr<tArtTypeCacheType> getArtMap(long id, std::string type);

    void addArtistArtMap(long id, std::shared_ptr<tArtTypeCacheType>& item, std::string type);
    std::shared_ptr<tArtTypeCacheType> getArtistArtMap(long id, std::string type);

    void addArtistArt(long id, std::shared_ptr<tArtistArtTypeCacheType>& item, std::string type);
    std::shared_ptr<tArtistArtTypeCacheType> getArtistArt(long id, std::string type);

    void addArtist(long id, std::shared_ptr<CFileItem>& item);
    std::shared_ptr<CFileItem> getArtist(long id);

    void addArtThumbLoader(int songId, int albumId, int artistId, int playlistId, bool bPrimaryArtist, std::vector<ArtForThumbLoader> &art);
    std::shared_ptr<std::vector<ArtForThumbLoader> > getArtThumbLoader(int songId, int albumId, int artistId, int playlistId, bool bPrimaryArtist);

    std::string getTranslation(std::string key, uint64_t updatedAt);
    void languageChange();

  private:
    void setCurrentLanguage();
    void loadTranslations();

    tMusicInfoTagCacheMap m_songCacheMap;
    tMusicInfoTagCacheMap m_albumCacheMap;
    tArtTypeCacheMap m_ArtCacheMap;
    tArtTypeCacheMap m_ArtistArtMapCacheMap;
    tArtistArtTypeCacheMap m_ArtistArtCacheMap;
    tFileItemCacheMap m_ArtistCacheMap;
    tArtThumbLoaderTypeMap m_ArtThumbLoaderCacheMap;
    tTranslationCacheMap m_TranslationCacheMap;

    std::string m_language;
    CCriticalSection m_mutex;
  };
}
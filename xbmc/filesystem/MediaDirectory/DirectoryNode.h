/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "utils/UrlOptions.h"

class CFileItemList;

namespace XFILE
{
  namespace MEDIADIRECTORY
  {
    class CQueryParams;

    typedef enum _NODE_TYPE
    {
      // common stuff
      NODE_TYPE_NONE=0,
      NODE_TYPE_ROOT,
      NODE_TYPE_OVERVIEW,
      NODE_TYPE_TOP100,
      NODE_TYPE_ROLE,
      NODE_TYPE_SOURCE,
      NODE_TYPE_GENRE,
      NODE_TYPE_MUSICGENRE, // not sure if needed
      NODE_TYPE_VIDEOGENRE, // not sure if needed
      // music stuff
      NODE_TYPE_ARTIST,
      NODE_TYPE_ALBUM,
      NODE_TYPE_ALBUM_RECENTLY_ADDED,
      NODE_TYPE_ALBUM_RECENTLY_ADDED_SONGS,
      NODE_TYPE_ALBUM_RECENTLY_PLAYED,
      NODE_TYPE_ALBUM_RECENTLY_PLAYED_SONGS,
      NODE_TYPE_ALBUM_TOP100,
      NODE_TYPE_ALBUM_TOP100_SONGS,
      NODE_TYPE_ALBUM_COMPILATIONS,
      NODE_TYPE_ALBUM_COMPILATIONS_SONGS,
      NODE_TYPE_SONG,
      NODE_TYPE_SONG_TOP100,
      NODE_TYPE_YEAR,
      NODE_TYPE_YEAR_ALBUM,
      NODE_TYPE_YEAR_SONG,
      NODE_TYPE_SINGLES,
      NODE_TYPE_PLAYLIST,
      // video stuff
      NODE_TYPE_MOVIES_OVERVIEW,
      NODE_TYPE_TVSHOWS_OVERVIEW,
      NODE_TYPE_ACTOR,
      NODE_TYPE_TITLE_MOVIES,
      NODE_TYPE_DIRECTOR,
      NODE_TYPE_TITLE_TVSHOWS,
      NODE_TYPE_SEASONS,
      NODE_TYPE_EPISODES,
      NODE_TYPE_RECENTLY_ADDED_MOVIES,
      NODE_TYPE_RECENTLY_ADDED_EPISODES,
      NODE_TYPE_STUDIO,
      NODE_TYPE_MUSICVIDEOS_OVERVIEW,
      NODE_TYPE_RECENTLY_ADDED_MUSICVIDEOS,
      NODE_TYPE_TITLE_MUSICVIDEOS,
      NODE_TYPE_MUSICVIDEOS_ALBUM,
      NODE_TYPE_SETS,
      NODE_TYPE_COUNTRY,
      NODE_TYPE_TAGS,
      NODE_TYPE_INPROGRESS_TVSHOWS
    } NODE_TYPE;

    typedef struct {
      NODE_TYPE   node;
      std::string id;
      int         label;
    } Node;

    class CDirectoryNode
    {
    public:
      static CDirectoryNode* ParseURL(const std::string& strPath);
      static void GetDatabaseInfo(const std::string& strPath, CQueryParams& params);
      virtual ~CDirectoryNode();

      NODE_TYPE GetType() const;

      bool GetChilds(CFileItemList& items);
      virtual NODE_TYPE GetChildType() const;
      virtual std::string GetLocalizedName() const;

      CDirectoryNode* GetParent() const;
      virtual bool CanCache() const;

      std::string BuildPath() const;

    protected:
      CDirectoryNode(NODE_TYPE Type, const std::string& strName, CDirectoryNode* pParent, const std::string& strOrigin);
      static CDirectoryNode* CreateNode(NODE_TYPE Type, const std::string& strName, CDirectoryNode* pParent, const std::string& strOrigin);

      void AddOptions(const std::string &options);
      void CollectQueryParams(CQueryParams& params) const;

      const std::string& GetName() const;
      int GetID() const;
      void RemoveParent();

      virtual bool GetContent(CFileItemList& items) const;

    private:
      NODE_TYPE m_Type;
      std::string m_strName;
      CDirectoryNode* m_pParent;
      CUrlOptions m_options;
      std::string m_origin;
    };
  }
}
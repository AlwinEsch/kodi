/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "filesystem/MediaDirectory/DirectoryNode.h"

namespace XFILE
{
  namespace MUSICDATABASEDIRECTORY
  {
    class CDirectoryNodePlaylist : public XFILE::MEDIADIRECTORY::CDirectoryNode
    {
    public:
      CDirectoryNodePlaylist(const std::string& strEntryName, XFILE::MEDIADIRECTORY::CDirectoryNode* pParent, const std::string& strOrigin);
    protected:
      XFILE::MEDIADIRECTORY::NODE_TYPE GetChildType() const override;
      bool GetContent(CFileItemList& items) const override;
      std::string GetLocalizedName() const override;
    };
  }
}
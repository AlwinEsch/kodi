/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "DirectoryNode.h"

namespace XFILE
{
  namespace MEDIADIRECTORY
  {
    class CDirectoryNodeGrouped : public CDirectoryNode
    {
    public:
      CDirectoryNodeGrouped(NODE_TYPE type, const std::string& strName, CDirectoryNode* pParent, const std::string& strOrigin);
    protected:
      NODE_TYPE GetChildType() const override;
      bool GetContent(CFileItemList& items) const override;
      std::string GetLocalizedName() const override;

    private:
      std::string GetContentType() const;
      std::string GetContentType(const CQueryParams &params) const;
      std::string GetMediaType(const CQueryParams &params) const;
      std::string GetMediaType() const;
    };
  }
}

/*
 *  Copyright (C) 2013 Team XBMC
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

class CFileItemList;

class IFileItemListModifier
{
public:
  IFileItemListModifier() = default;
  virtual ~IFileItemListModifier() = default;

  virtual bool CanModify(const CFileItemList &items) const = 0;
  virtual bool Modify(CFileItemList &items) const = 0;
};

/*
 *  Copyright (C) 2012-2013 Team XBMC
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "InputStreamPVRBase.h"

class CInputStreamPVRRecording : public CInputStreamPVRBase
{
public:
  CInputStreamPVRRecording(IVideoPlayer* pPlayer, const CFileItem& fileitem);
  ~CInputStreamPVRRecording() override;

protected:
  bool OpenPVRStream() override;
  void ClosePVRStream() override;
  int ReadPVRStream(uint8_t* buf, int buf_size) override;
  int64_t SeekPVRStream(int64_t offset, int whence) override;
  int64_t GetPVRStreamLength() override;
  ENextStream NextPVRStream() override;
  bool CanPausePVRStream() override;
  bool CanSeekPVRStream() override;
};

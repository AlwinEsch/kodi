/*
 *  Copyright (C) 2005-2019 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "AddonBase.h"

#include <vector>

#ifndef BUILD_KODI_ADDON
#include "video/VideoInfoTag.h"
#endif

extern "C"
{
struct VFSInfoTagVideo
{
  int dbId;
  char** director;
  unsigned int directorCnt;
  char** writingCredits;
  unsigned int writingCreditsCnt;
  char** genres;
  unsigned int genresCnt;
  char** country;
  unsigned int countryCnt;
  char* tagLine;
  char* plotOutline;
  char* plot;
  char* trailer;
  char* pictureURL;
  char* title;
  char* sortTitle;
  char* tvShowTitle;
  char* mediaType;
  int votes;
  char* cast;
  char* file;
  char* path;
  char* imdbNumber;
  int season;
  int episode;
  int year;
  double rating;
  int userRating;
  int playCount;
  time_t lastPlayed;
  char* originalTitle;
  time_t premiered;
  time_t firstAired;
  char** artist;
  unsigned int artistCnt;
  char* album;
  int track;
  unsigned int duration;
};
} /* extern "C" */

namespace kodi
{
namespace vfs
{

#ifndef BUILD_KODI_ADDON
void TransToKodiVideoInfoTag(const VFSInfoTagVideo* c_tag, CVideoInfoTag& tag)
{
  tag.m_iDbId = c_tag->dbId;
  tag.m_strTagLine = c_tag->tagLine;
  tag.m_strPlotOutline = c_tag->plotOutline;
  tag.m_strPlot = c_tag->plot;
  tag.m_strTrailer = c_tag->trailer;
  tag.m_strPictureURL = c_tag->pictureURL;
  tag.m_strTitle = c_tag->title;
  tag.m_strSortTitle = c_tag->sortTitle;
  tag.m_strShowTitle = c_tag->tvShowTitle;
  tag.m_type = c_tag->mediaType;
  tag.m_iTrack = c_tag->track;
  tag.m_strAlbum = c_tag->album;
  tag.m_firstAired = c_tag->firstAired;
  tag.m_lastPlayed = c_tag->lastPlayed;
  tag.m_iSeason = c_tag->season;
  tag.m_iEpisode = c_tag->episode;

  for (unsigned int i = 0; i < c_tag->directorCnt; ++i)
    tag.m_director.push_back(c_tag->director[i]);
  for (unsigned int i = 0; i < c_tag->writingCreditsCnt; ++i)
    tag.m_writingCredits.push_back(c_tag->writingCredits[i]);
  for (unsigned int i = 0; i < c_tag->genresCnt; ++i)
    tag.m_genre.push_back(c_tag->genres[i]);
  for (unsigned int i = 0; i < c_tag->countryCnt; ++i)
    tag.m_country.push_back(c_tag->country[i]);
  for (unsigned int i = 0; i < c_tag->artistCnt; ++i)
    tag.m_country.push_back(c_tag->artist[i]);

  tag.SetFile(c_tag->file);
  tag.SetPath(c_tag->path);
  tag.SetVotes(c_tag->votes);
  tag.SetDuration(c_tag->duration);
  tag.SetPremiered(c_tag->premiered);
  tag.SetOriginalTitle(c_tag->originalTitle);
  tag.SetPlayCount(c_tag->playCount);
  tag.SetUserrating(c_tag->userRating);
  tag.SetRating(c_tag->rating);
  tag.SetYear(c_tag->year);
  tag.SetUniqueID(c_tag->imdbNumber);


  // TODO: char* c_tag->cast
}
#endif

class CInfoTagVideo : public kodi::addon::CStructHdl<CInfoTagVideo, VFSInfoTagVideo>
{
public:
  CInfoTagVideo()
  {
    memset(m_cStructure, 0, sizeof(VFSInfoTagVideo));
  }
  CInfoTagVideo(VFSInfoTagVideo* sample) : CStructHdl(sample) { }
  ~CInfoTagVideo()
  {
    if (IsOwner())
    {
      std::vector<std::string> dummy;

      FreeStringList(m_cStructure->director, m_cStructure->directorCnt);
      FreeStringList(m_cStructure->writingCredits, m_cStructure->writingCreditsCnt);
      FreeStringList(m_cStructure->genres, m_cStructure->genresCnt);
      FreeStringList(m_cStructure->country, m_cStructure->countryCnt);
      FreeString(m_cStructure->tagLine);
      FreeString(m_cStructure->plotOutline);
      FreeString(m_cStructure->plot);
      FreeString(m_cStructure->trailer);
      FreeString(m_cStructure->pictureURL);
      FreeString(m_cStructure->title);
      FreeString(m_cStructure->sortTitle);
      FreeString(m_cStructure->tvShowTitle);
      FreeString(m_cStructure->mediaType);
      FreeString(m_cStructure->cast);
      FreeString(m_cStructure->file);
      FreeString(m_cStructure->path);
      FreeString(m_cStructure->imdbNumber);
      FreeString(m_cStructure->originalTitle);
      FreeStringList(m_cStructure->artist, m_cStructure->artistCnt);
      FreeString(m_cStructure->album);
    }
  }

  void SetDbId(int dbId) { m_cStructure->dbId = dbId; }
  int GetDbId() const { return m_cStructure->dbId; }

  void SetDirector(const std::vector<std::string>& director) { SetStringList(m_cStructure->director, m_cStructure->directorCnt, director); }
  std::vector<std::string> GetDirector() const { return GetStringList(m_cStructure->director, m_cStructure->directorCnt); }

  void SetWritingCredits(const std::vector<std::string>& writingCredits) { SetStringList(m_cStructure->writingCredits, m_cStructure->writingCreditsCnt, writingCredits); }
  std::vector<std::string> GetWritingCredits() const { return GetStringList(m_cStructure->writingCredits, m_cStructure->writingCreditsCnt); }

  void SetGenre(const std::vector<std::string>& genres) { SetStringList(m_cStructure->genres, m_cStructure->genresCnt, genres); }
  std::vector<std::string> GetGenre() const { return GetStringList(m_cStructure->genres, m_cStructure->genresCnt); }

  void SetCountry(const std::vector<std::string>& country) { SetStringList(m_cStructure->country, m_cStructure->countryCnt, country); }
  std::vector<std::string> GetCountry() const { return GetStringList(m_cStructure->country, m_cStructure->countryCnt); }

  void SetTagLine(const std::string& tagLine) { SetString(m_cStructure->tagLine, tagLine); }
  std::string GetTagLine() const { return GetString(m_cStructure->tagLine); }

  void SetPlotOutline(const std::string& plotOutline) { SetString(m_cStructure->plotOutline, plotOutline); }
  std::string GetPlotOutline() const { return GetString(m_cStructure->plotOutline); }

  void SetPlot(const std::string& plot) { SetString(m_cStructure->plot, plot); }
  std::string GetPlot() const { return GetString(m_cStructure->plot); }

  void SetTrailer(const std::string& trailer) { SetString(m_cStructure->trailer, trailer); }
  std::string GetTrailer() const { return GetString(m_cStructure->trailer); }

  void SetPictureURL(const std::string& pictureURL) { SetString(m_cStructure->pictureURL, pictureURL); }
  std::string GetPictureURL() const { return GetString(m_cStructure->pictureURL); }

  void SetTitle(const std::string& title) { SetString(m_cStructure->title, title); }
  std::string GetTitle() const { return GetString(m_cStructure->title); }

  void SetSortTitle(const std::string& sortTitle) { SetString(m_cStructure->sortTitle, sortTitle); }
  std::string GetSortTitle() const { return GetString(m_cStructure->sortTitle); }

  void SetTVShowTitle(const std::string& tvShowTitle) { SetString(m_cStructure->tvShowTitle, tvShowTitle); }
  std::string GetTVShowTitle() const { return GetString(m_cStructure->tvShowTitle); }

  void SetMediaType(const std::string& mediaType) { SetString(m_cStructure->mediaType, mediaType); }
  std::string GetMediaType() const { return GetString(m_cStructure->mediaType); }

  void SetVotes(int votes) { m_cStructure->votes = votes; }
  int GetVotes() const { return m_cStructure->votes; }

  void SetCast(const std::string& cast) { SetString(m_cStructure->cast, cast); }
  std::string GetCast() const { return GetString(m_cStructure->cast); }

  void SetFile(const std::string& file) { SetString(m_cStructure->file, file); }
  std::string GetFile() const { return GetString(m_cStructure->file); }

  void SetPath(const std::string& path) { SetString(m_cStructure->path, path); }
  std::string GetPath() const { return GetString(m_cStructure->path); }

  void SetIMDBNumber(const std::string& imdbNumber) { SetString(m_cStructure->imdbNumber, imdbNumber); }
  std::string GetIMDBNumber() const { return GetString(m_cStructure->imdbNumber); }

  void SetSeason(int season) { m_cStructure->season = season; }
  int GetSeason() const { return m_cStructure->season; }

  void SetEpisode(int episode) { m_cStructure->episode = episode; }
  int GetEpisode() const { return m_cStructure->episode; }

  void SetYear(int year) { m_cStructure->year = year; }
  int GetYear() const { return m_cStructure->year; }

  void SetRating(double rating) { m_cStructure->rating = rating; }
  double GetRating() const { return m_cStructure->rating; }

  void SetUserRating(int userRating) { m_cStructure->userRating = userRating; }
  int GetUserRating() const { return m_cStructure->userRating; }

  void SetPlayCount(int playCount) { m_cStructure->playCount = playCount; }
  int GetPlayCount() const { return m_cStructure->playCount; }

  void SetLastPlayed(time_t lastPlayed) { m_cStructure->lastPlayed = lastPlayed; }
  time_t GetLastPlayed() const { return m_cStructure->lastPlayed; }

  void SetOriginalTitle(const std::string& originalTitle) { SetString(m_cStructure->originalTitle, originalTitle); }
  std::string GetOriginalTitle() const { return GetString(m_cStructure->originalTitle); }

  void SetPremiered(time_t premiered) { m_cStructure->premiered = premiered; }
  time_t GetPremiered() const { return m_cStructure->premiered; }

  void SetFirstAired(time_t firstAired) { m_cStructure->firstAired = firstAired; }
  time_t GetFirstAired() const { return m_cStructure->firstAired; }

  void SetArtist(const std::vector<std::string>& artist) { SetStringList(m_cStructure->artist, m_cStructure->artistCnt, artist); }
  std::vector<std::string> GetArtist() const { return GetStringList(m_cStructure->artist, m_cStructure->artistCnt); }

  void SetAlbum(const std::string& album) { SetString(m_cStructure->album, album); }
  std::string GetAlbum() const { return GetString(m_cStructure->album); }

  void SetTrack(int track) { m_cStructure->track = track; }
  int GetTrack() const { return m_cStructure->track; }

  void SetDuration(unsigned int duration) { m_cStructure->duration = duration; }
  unsigned int GetDuration() const { return m_cStructure->duration; }
};

} /* namespace vfs */
} /* namespace kodi */

/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PVRClient.h"

#include "ServiceBroker.h"
#include "addons/AddonManager.h"
#include "cores/VideoPlayer/DVDDemuxers/DVDDemuxUtils.h"
#include "dialogs/GUIDialogKaiToast.h" //! @todo get rid of GUI in core
#include "events/EventLog.h"
#include "events/NotificationEvent.h"
#include "filesystem/SpecialProtocol.h"
#include "guilib/LocalizeStrings.h"
#include "pvr/PVRDatabase.h"
#include "pvr/PVRManager.h"
#include "pvr/PVRStreamProperties.h"
#include "pvr/addons/PVRClientMenuHooks.h"
#include "pvr/addons/PVRClients.h"
#include "pvr/channels/PVRChannel.h"
#include "pvr/channels/PVRChannelGroup.h"
#include "pvr/channels/PVRChannelGroupInternal.h"
#include "pvr/channels/PVRChannelGroupMember.h"
#include "pvr/channels/PVRChannelGroups.h"
#include "pvr/channels/PVRChannelGroupsContainer.h"
#include "pvr/epg/Epg.h"
#include "pvr/epg/EpgContainer.h"
#include "pvr/epg/EpgInfoTag.h"
#include "pvr/providers/PVRProvider.h"
#include "pvr/providers/PVRProviders.h"
#include "pvr/recordings/PVRRecording.h"
#include "pvr/recordings/PVRRecordings.h"
#include "pvr/timers/PVRTimerInfoTag.h"
#include "pvr/timers/PVRTimerType.h"
#include "pvr/timers/PVRTimers.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "utils/StringUtils.h"
#include "utils/log.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

extern "C"
{
#include <libavcodec/avcodec.h>
}

using namespace ADDON;
using namespace KODI::ADDONS::INTERFACE;

namespace PVR
{

#define DEFAULT_INFO_STRING_VALUE "unknown"

CPVRClient::CPVRClient(const ADDON::AddonInfoPtr& addonInfo,
                       ADDON::AddonInstanceId instanceId,
                       int clientId)
  : IInstanceHandler(this, ADDON_INSTANCE_PVR, addonInfo, instanceId), m_iClientId(clientId)
{
  ResetProperties();
}

CPVRClient::~CPVRClient()
{
  Destroy();
}

void CPVRClient::StopRunningInstance()
{
  // stop the pvr manager and stop and unload the running pvr addon. pvr manager will be restarted on demand.
  CServiceBroker::GetPVRManager().Stop();
  CServiceBroker::GetPVRManager().Clients()->StopClient(m_iClientId, false);
}

void CPVRClient::OnPreInstall()
{
  // note: this method is also called on update; thus stop and unload possibly running instance
  StopRunningInstance();
}

void CPVRClient::OnPreUnInstall()
{
  StopRunningInstance();
}

void CPVRClient::ResetProperties()
{
  std::unique_lock<CCriticalSection> lock(m_critSection);

  /* initialise members */
  m_strUserPath = CSpecialProtocol::TranslatePath(Profile());
  m_strClientPath = CSpecialProtocol::TranslatePath(Path());
  m_bReadyToUse = false;
  m_bBlockAddonCalls = false;
  m_iAddonCalls = 0;
  m_allAddonCallsFinished.Set();
  m_connectionState = PVR_CONNECTION_STATE_UNKNOWN;
  m_prevConnectionState = PVR_CONNECTION_STATE_UNKNOWN;
  m_ignoreClient = false;
  m_iPriority = 0;
  m_bPriorityFetched = false;
  m_strBackendVersion = DEFAULT_INFO_STRING_VALUE;
  m_strConnectionString = DEFAULT_INFO_STRING_VALUE;
  m_strFriendlyName = DEFAULT_INFO_STRING_VALUE;
  m_strBackendName = DEFAULT_INFO_STRING_VALUE;
  m_strBackendHostname.clear();
  m_menuhooks.reset();
  m_timertypes.clear();
  m_clientCapabilities.clear();
}

ADDON_STATUS CPVRClient::Create()
{
  ADDON_STATUS status(ADDON_STATUS_UNKNOWN);

  ResetProperties();

  /* initialise the add-on */
  CLog::LogFC(LOGDEBUG, LOGPVR, "Creating PVR add-on instance '{}'", ID());

  bool bReadyToUse = false;
  if ((status = CreateInstance()) == ADDON_STATUS_OK)
    bReadyToUse = GetAddonProperties();

  CLog::LogFC(LOGDEBUG, LOGPVR, "Created PVR add-on instance '{}'. readytouse={} ", ID(),
              bReadyToUse);

  m_bReadyToUse = bReadyToUse;
  return status;
}

void CPVRClient::Destroy()
{
  if (!m_bReadyToUse)
    return;

  m_bReadyToUse = false;

  CLog::LogFC(LOGDEBUG, LOGPVR, "Destroying PVR add-on instance '{}'", ID());

  m_bBlockAddonCalls = true;
  m_allAddonCallsFinished.Wait();

  DestroyInstance();

  CLog::LogFC(LOGDEBUG, LOGPVR, "Destroyed PVR add-on instance '{}'", ID());

  if (m_menuhooks)
    m_menuhooks->Clear();

  ResetProperties();
}

void CPVRClient::Stop()
{
  m_bBlockAddonCalls = true;
  m_bPriorityFetched = false;
}

void CPVRClient::Continue()
{
  m_bBlockAddonCalls = false;
}

void CPVRClient::ReCreate()
{
  Destroy();
  Create();
}

bool CPVRClient::ReadyToUse() const
{
  return m_bReadyToUse;
}

PVR_CONNECTION_STATE CPVRClient::GetConnectionState() const
{
  std::unique_lock<CCriticalSection> lock(m_critSection);
  return m_connectionState;
}

void CPVRClient::SetConnectionState(PVR_CONNECTION_STATE state)
{
  if (state == PVR_CONNECTION_STATE_CONNECTED)
  {
    // update properties - some will only be available after add-on is connected to backend
    if (!GetAddonProperties())
      CLog::LogF(LOGERROR, "Error reading PVR client properties");
  }

  std::unique_lock<CCriticalSection> lock(m_critSection);

  m_prevConnectionState = m_connectionState;
  m_connectionState = state;

  if (m_connectionState == PVR_CONNECTION_STATE_CONNECTED)
    m_ignoreClient = false;
  else if (m_connectionState == PVR_CONNECTION_STATE_CONNECTING &&
           m_prevConnectionState == PVR_CONNECTION_STATE_UNKNOWN)
    m_ignoreClient = true; // ignore until connected
}

PVR_CONNECTION_STATE CPVRClient::GetPreviousConnectionState() const
{
  std::unique_lock<CCriticalSection> lock(m_critSection);
  return m_prevConnectionState;
}

bool CPVRClient::IgnoreClient() const
{
  std::unique_lock<CCriticalSection> lock(m_critSection);
  return m_ignoreClient;
}

bool CPVRClient::IsEnabled() const
{
  if (InstanceId() == ADDON_SINGLETON_INSTANCE_ID)
  {
    return !CServiceBroker::GetAddonMgr().IsAddonDisabled(ID());
  }
  else
  {
    bool instanceEnabled{false};
    Addon()->ReloadSettings(InstanceId());
    Addon()->GetSettingBool(ADDON_SETTING_INSTANCE_ENABLED_VALUE, instanceEnabled, InstanceId());
    return instanceEnabled;
  }
}

int CPVRClient::GetID() const
{
  return m_iClientId;
}

/*!
 * @brief Copy over group info from xbmcGroup to addonGroup.
 * @param xbmcGroup The group on XBMC's side.
 * @param addonGroup The group on the addon's side.
 */
void CPVRClient::WriteClientGroupInfo(const CPVRChannelGroup& xbmcGroup,
                                      PVR_CHANNEL_GROUP& addonGroup)
{
  addonGroup = {};
  addonGroup.bIsRadio = xbmcGroup.IsRadio();
  strncpy(addonGroup.strGroupName, xbmcGroup.GroupName().c_str(),
          sizeof(addonGroup.strGroupName) - 1);
}

/*!
 * @brief Copy over recording info from xbmcRecording to addonRecording.
 * @param xbmcRecording The recording on XBMC's side.
 * @param addonRecording The recording on the addon's side.
 */
void CPVRClient::WriteClientRecordingInfo(const CPVRRecording& xbmcRecording,
                                          PVR_RECORDING& addonRecording)
{
  time_t recTime;
  xbmcRecording.RecordingTimeAsUTC().GetAsTime(recTime);

  addonRecording = {};
  strncpy(addonRecording.strRecordingId, xbmcRecording.m_strRecordingId.c_str(),
          sizeof(addonRecording.strRecordingId) - 1);
  strncpy(addonRecording.strTitle, xbmcRecording.m_strTitle.c_str(),
          sizeof(addonRecording.strTitle) - 1);
  strncpy(addonRecording.strEpisodeName, xbmcRecording.m_strShowTitle.c_str(),
          sizeof(addonRecording.strEpisodeName) - 1);
  addonRecording.iSeriesNumber = xbmcRecording.m_iSeason;
  addonRecording.iEpisodeNumber = xbmcRecording.m_iEpisode;
  addonRecording.iYear = xbmcRecording.GetYear();
  strncpy(addonRecording.strDirectory, xbmcRecording.m_strDirectory.c_str(),
          sizeof(addonRecording.strDirectory) - 1);
  strncpy(addonRecording.strPlotOutline, xbmcRecording.m_strPlotOutline.c_str(),
          sizeof(addonRecording.strPlotOutline) - 1);
  strncpy(addonRecording.strPlot, xbmcRecording.m_strPlot.c_str(),
          sizeof(addonRecording.strPlot) - 1);
  strncpy(addonRecording.strGenreDescription, xbmcRecording.GetGenresLabel().c_str(),
          sizeof(addonRecording.strGenreDescription) - 1);
  strncpy(addonRecording.strChannelName, xbmcRecording.m_strChannelName.c_str(),
          sizeof(addonRecording.strChannelName) - 1);
  strncpy(addonRecording.strIconPath, xbmcRecording.ClientIconPath().c_str(),
          sizeof(addonRecording.strIconPath) - 1);
  strncpy(addonRecording.strThumbnailPath, xbmcRecording.ClientThumbnailPath().c_str(),
          sizeof(addonRecording.strThumbnailPath) - 1);
  strncpy(addonRecording.strFanartPath, xbmcRecording.ClientFanartPath().c_str(),
          sizeof(addonRecording.strFanartPath) - 1);
  addonRecording.recordingTime =
      recTime - CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_iPVRTimeCorrection;
  addonRecording.iDuration = xbmcRecording.GetDuration();
  addonRecording.iPriority = xbmcRecording.m_iPriority;
  addonRecording.iLifetime = xbmcRecording.m_iLifetime;
  addonRecording.iGenreType = xbmcRecording.GenreType();
  addonRecording.iGenreSubType = xbmcRecording.GenreSubType();
  addonRecording.iPlayCount = xbmcRecording.GetLocalPlayCount();
  addonRecording.iLastPlayedPosition = lrint(xbmcRecording.GetLocalResumePoint().timeInSeconds);
  addonRecording.bIsDeleted = xbmcRecording.IsDeleted();
  addonRecording.iChannelUid = xbmcRecording.ChannelUid();
  addonRecording.channelType =
      xbmcRecording.IsRadio() ? PVR_RECORDING_CHANNEL_TYPE_RADIO : PVR_RECORDING_CHANNEL_TYPE_TV;
  if (xbmcRecording.FirstAired().IsValid())
    strncpy(addonRecording.strFirstAired, xbmcRecording.FirstAired().GetAsW3CDate().c_str(),
            sizeof(addonRecording.strFirstAired) - 1);
  strncpy(addonRecording.strProviderName, xbmcRecording.ProviderName().c_str(),
          sizeof(addonRecording.strProviderName) - 1);
  addonRecording.iClientProviderUid = xbmcRecording.ClientProviderUniqueId();
}

/*!
 * @brief Copy over timer info from xbmcTimer to addonTimer.
 * @param xbmcTimer The timer on XBMC's side.
 * @param addonTimer The timer on the addon's side.
 */
void CPVRClient::WriteClientTimerInfo(const CPVRTimerInfoTag& xbmcTimer, PVR_TIMER& addonTimer)
{
  time_t start, end, firstDay;
  xbmcTimer.StartAsUTC().GetAsTime(start);
  xbmcTimer.EndAsUTC().GetAsTime(end);
  xbmcTimer.FirstDayAsUTC().GetAsTime(firstDay);
  std::shared_ptr<CPVREpgInfoTag> epgTag = xbmcTimer.GetEpgInfoTag();

  int iPVRTimeCorrection =
      CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_iPVRTimeCorrection;

  addonTimer = {};
  addonTimer.iClientIndex = xbmcTimer.m_iClientIndex;
  addonTimer.iParentClientIndex = xbmcTimer.m_iParentClientIndex;
  addonTimer.state = xbmcTimer.m_state;
  addonTimer.iTimerType = xbmcTimer.GetTimerType()->GetTypeId();
  addonTimer.iClientChannelUid = xbmcTimer.m_iClientChannelUid;
  strncpy(addonTimer.strTitle, xbmcTimer.m_strTitle.c_str(), sizeof(addonTimer.strTitle) - 1);
  strncpy(addonTimer.strEpgSearchString, xbmcTimer.m_strEpgSearchString.c_str(),
          sizeof(addonTimer.strEpgSearchString) - 1);
  addonTimer.bFullTextEpgSearch = xbmcTimer.m_bFullTextEpgSearch;
  strncpy(addonTimer.strDirectory, xbmcTimer.m_strDirectory.c_str(),
          sizeof(addonTimer.strDirectory) - 1);
  addonTimer.iPriority = xbmcTimer.m_iPriority;
  addonTimer.iLifetime = xbmcTimer.m_iLifetime;
  addonTimer.iMaxRecordings = xbmcTimer.m_iMaxRecordings;
  addonTimer.iPreventDuplicateEpisodes = xbmcTimer.m_iPreventDupEpisodes;
  addonTimer.iRecordingGroup = xbmcTimer.m_iRecordingGroup;
  addonTimer.iWeekdays = xbmcTimer.m_iWeekdays;
  addonTimer.startTime = start - iPVRTimeCorrection;
  addonTimer.endTime = end - iPVRTimeCorrection;
  addonTimer.bStartAnyTime = xbmcTimer.m_bStartAnyTime;
  addonTimer.bEndAnyTime = xbmcTimer.m_bEndAnyTime;
  addonTimer.firstDay = firstDay - iPVRTimeCorrection;
  addonTimer.iEpgUid = epgTag ? epgTag->UniqueBroadcastID() : PVR_TIMER_NO_EPG_UID;
  strncpy(addonTimer.strSummary, xbmcTimer.m_strSummary.c_str(), sizeof(addonTimer.strSummary) - 1);
  addonTimer.iMarginStart = xbmcTimer.m_iMarginStart;
  addonTimer.iMarginEnd = xbmcTimer.m_iMarginEnd;
  addonTimer.iGenreType = epgTag ? epgTag->GenreType() : 0;
  addonTimer.iGenreSubType = epgTag ? epgTag->GenreSubType() : 0;
  strncpy(addonTimer.strSeriesLink, xbmcTimer.SeriesLink().c_str(),
          sizeof(addonTimer.strSeriesLink) - 1);
}

/*!
 * @brief Copy over channel info from xbmcChannel to addonClient.
 * @param xbmcChannel The channel on XBMC's side.
 * @param addonChannel The channel on the addon's side.
 */
void CPVRClient::WriteClientChannelInfo(const std::shared_ptr<CPVRChannel>& xbmcChannel,
                                        PVR_CHANNEL& addonChannel)
{
  addonChannel = {};
  addonChannel.iUniqueId = xbmcChannel->UniqueID();
  addonChannel.iChannelNumber = xbmcChannel->ClientChannelNumber().GetChannelNumber();
  addonChannel.iSubChannelNumber = xbmcChannel->ClientChannelNumber().GetSubChannelNumber();
  strncpy(addonChannel.strChannelName, xbmcChannel->ClientChannelName().c_str(),
          sizeof(addonChannel.strChannelName) - 1);
  strncpy(addonChannel.strIconPath, xbmcChannel->ClientIconPath().c_str(),
          sizeof(addonChannel.strIconPath) - 1);
  addonChannel.iEncryptionSystem = xbmcChannel->EncryptionSystem();
  addonChannel.bIsRadio = xbmcChannel->IsRadio();
  addonChannel.bIsHidden = xbmcChannel->IsHidden();
  strncpy(addonChannel.strMimeType, xbmcChannel->MimeType().c_str(),
          sizeof(addonChannel.strMimeType) - 1);
  addonChannel.iClientProviderUid = xbmcChannel->ClientProviderUid();
}

bool CPVRClient::GetAddonProperties()
{
  char strBackendName[PVR_ADDON_NAME_STRING_LENGTH] = {};
  char strConnectionString[PVR_ADDON_NAME_STRING_LENGTH] = {};
  char strBackendVersion[PVR_ADDON_NAME_STRING_LENGTH] = {};
  char strBackendHostname[PVR_ADDON_NAME_STRING_LENGTH] = {};
  std::string strFriendlyName;
  PVR_ADDON_CAPABILITIES addonCapabilities = {};
  std::vector<std::shared_ptr<CPVRTimerType>> timerTypes;

  /* get the capabilities */
  PVR_ERROR retVal = DoAddonCall(
      __func__,
      [this, &addonCapabilities](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_capabilities_v1(
            addon, &addonCapabilities);
      },
      true, false);

  if (retVal != PVR_ERROR_NO_ERROR)
    return false;

  /* get the name of the backend */
  retVal = DoAddonCall(
      __func__,
      [this, &strBackendName](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_backend_name_v1(
            addon, strBackendName, sizeof(strBackendName));
      },
      true, false);

  if (retVal != PVR_ERROR_NO_ERROR)
    return false;

  /* get the connection string */
  retVal = DoAddonCall(
      __func__,
      [this, &strConnectionString](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_connection_string_v1(
            addon, strConnectionString, sizeof(strConnectionString));
      },
      true, false);

  if (retVal != PVR_ERROR_NO_ERROR && retVal != PVR_ERROR_NOT_IMPLEMENTED)
    return false;

  /* display name = backend name:connection string */
  strFriendlyName = strlen(strConnectionString) == 0 // connection string is optional
                        ? strBackendName
                        : StringUtils::Format("{}:{}", strBackendName, strConnectionString);

  /* backend version number */
  retVal = DoAddonCall(
      __func__,
      [this, &strBackendVersion](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_backend_version_v1(
            addon, strBackendVersion, sizeof(strBackendVersion));
      },
      true, false);

  if (retVal != PVR_ERROR_NO_ERROR)
    return false;

  /* backend hostname */
  retVal = DoAddonCall(
      __func__,
      [this, &strBackendHostname](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_backend_hostname_v1(
            addon, strBackendHostname, sizeof(strBackendHostname));
      },
      true, false);

  if (retVal != PVR_ERROR_NO_ERROR && retVal != PVR_ERROR_NOT_IMPLEMENTED)
    return false;

  /* timer types */
  retVal = DoAddonCall(
      __func__,
      [this, strFriendlyName, &addonCapabilities, &timerTypes](KODI_ADDON_PVR_HDL addon)
      {
        std::unique_ptr<PVR_TIMER_TYPE[]> types_array(
            new PVR_TIMER_TYPE[PVR_ADDON_TIMERTYPE_ARRAY_SIZE]);
        size_t size = PVR_ADDON_TIMERTYPE_ARRAY_SIZE;

        PVR_ERROR retval = m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_timer_types_v1(
            addon, types_array.get(), &size);

        if (retval == PVR_ERROR_NOT_IMPLEMENTED)
        {
          // begin compat section
          CLog::LogF(LOGWARNING,
                     "Add-on {} does not support timer types. It will work, but not benefit from "
                     "the timer features introduced with PVR Addon API 2.0.0",
                     strFriendlyName);

          // Create standard timer types (mostly) matching the timer functionality available in Isengard.
          // This is for migration only and does not make changes to the addons obsolete. Addons should
          // work and benefit from some UI changes (e.g. some of the timer settings dialog enhancements),
          // but all old problems/bugs due to static attributes and values will remain the same as in
          // Isengard. Also, new features (like epg search) are not available to addons automatically.
          // This code can be removed once all addons actually support the respective PVR Addon API version.

          size = 0;
          // manual one time
          memset(&types_array[size], 0, sizeof(types_array[size]));
          types_array[size].iId = size + 1;
          types_array[size].iAttributes =
              PVR_TIMER_TYPE_IS_MANUAL | PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE |
              PVR_TIMER_TYPE_SUPPORTS_CHANNELS | PVR_TIMER_TYPE_SUPPORTS_START_TIME |
              PVR_TIMER_TYPE_SUPPORTS_END_TIME | PVR_TIMER_TYPE_SUPPORTS_PRIORITY |
              PVR_TIMER_TYPE_SUPPORTS_LIFETIME | PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS;
          ++size;

          // manual timer rule
          memset(&types_array[size], 0, sizeof(types_array[size]));
          types_array[size].iId = size + 1;
          types_array[size].iAttributes =
              PVR_TIMER_TYPE_IS_MANUAL | PVR_TIMER_TYPE_IS_REPEATING |
              PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE | PVR_TIMER_TYPE_SUPPORTS_CHANNELS |
              PVR_TIMER_TYPE_SUPPORTS_START_TIME | PVR_TIMER_TYPE_SUPPORTS_END_TIME |
              PVR_TIMER_TYPE_SUPPORTS_PRIORITY | PVR_TIMER_TYPE_SUPPORTS_LIFETIME |
              PVR_TIMER_TYPE_SUPPORTS_FIRST_DAY | PVR_TIMER_TYPE_SUPPORTS_WEEKDAYS |
              PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS;
          ++size;

          if (addonCapabilities.bSupportsEPG)
          {
            // One-shot epg-based
            memset(&types_array[size], 0, sizeof(types_array[size]));
            types_array[size].iId = size + 1;
            types_array[size].iAttributes =
                PVR_TIMER_TYPE_SUPPORTS_ENABLE_DISABLE | PVR_TIMER_TYPE_REQUIRES_EPG_TAG_ON_CREATE |
                PVR_TIMER_TYPE_SUPPORTS_CHANNELS | PVR_TIMER_TYPE_SUPPORTS_START_TIME |
                PVR_TIMER_TYPE_SUPPORTS_END_TIME | PVR_TIMER_TYPE_SUPPORTS_PRIORITY |
                PVR_TIMER_TYPE_SUPPORTS_LIFETIME | PVR_TIMER_TYPE_SUPPORTS_RECORDING_FOLDERS;
            ++size;
          }

          retval = PVR_ERROR_NO_ERROR;
          // end compat section
        }

        if (retval == PVR_ERROR_NO_ERROR)
        {
          timerTypes.reserve(size);
          for (size_t i = 0; i < size; ++i)
          {
            if (types_array[i].iId == PVR_TIMER_TYPE_NONE)
            {
              CLog::LogF(LOGERROR, "Invalid timer type supplied by add-on '{}'.", ID());
              continue;
            }
            timerTypes.emplace_back(
                std::shared_ptr<CPVRTimerType>(new CPVRTimerType(types_array[i], m_iClientId)));
          }
        }
        return retval;
      },
      addonCapabilities.bSupportsTimers, false);

  if (retVal == PVR_ERROR_NOT_IMPLEMENTED)
    retVal = PVR_ERROR_NO_ERROR; // timer support is optional.

  /* update the members */
  std::unique_lock<CCriticalSection> lock(m_critSection);
  m_strBackendName = strBackendName;
  m_strConnectionString = strConnectionString;
  m_strFriendlyName = strFriendlyName;
  m_strBackendVersion = strBackendVersion;
  m_clientCapabilities = addonCapabilities;
  m_strBackendHostname = strBackendHostname;
  m_timertypes = timerTypes;

  return retVal == PVR_ERROR_NO_ERROR;
}

const std::string& CPVRClient::GetBackendName() const
{
  return m_strBackendName;
}

const std::string& CPVRClient::GetBackendVersion() const
{
  return m_strBackendVersion;
}

const std::string& CPVRClient::GetBackendHostname() const
{
  return m_strBackendHostname;
}

const std::string& CPVRClient::GetConnectionString() const
{
  return m_strConnectionString;
}

const std::string& CPVRClient::GetFriendlyName() const
{
  return m_strFriendlyName;
}

PVR_ERROR CPVRClient::GetDriveSpace(uint64_t& iTotal, uint64_t& iUsed)
{
  /* default to 0 in case of error */
  iTotal = 0;
  iUsed = 0;

  return DoAddonCall(__func__,
                     [this, &iTotal, &iUsed](KODI_ADDON_PVR_HDL addon)
                     {
                       uint64_t iTotalSpace = 0;
                       uint64_t iUsedSpace = 0;
                       PVR_ERROR error =
                           m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_drive_space_v1(
                               addon, &iTotalSpace, &iUsedSpace);
                       if (error == PVR_ERROR_NO_ERROR)
                       {
                         iTotal = iTotalSpace;
                         iUsed = iUsedSpace;
                       }
                       return error;
                     });
}

PVR_ERROR CPVRClient::StartChannelScan()
{
  return DoAddonCall(
      __func__,
      [this](KODI_ADDON_PVR_HDL addon) {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_open_dialog_channel_scan_v1(addon);
      },
      m_clientCapabilities.SupportsChannelScan());
}

PVR_ERROR CPVRClient::OpenDialogChannelAdd(const std::shared_ptr<CPVRChannel>& channel)
{
  return DoAddonCall(
      __func__,
      [this, channel](KODI_ADDON_PVR_HDL addon)
      {
        PVR_CHANNEL addonChannel;
        WriteClientChannelInfo(channel, addonChannel);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_open_dialog_channel_add_v1(
            addon, &addonChannel);
      },
      m_clientCapabilities.SupportsChannelSettings());
}

PVR_ERROR CPVRClient::OpenDialogChannelSettings(const std::shared_ptr<CPVRChannel>& channel)
{
  return DoAddonCall(
      __func__,
      [this, channel](KODI_ADDON_PVR_HDL addon)
      {
        PVR_CHANNEL addonChannel;
        WriteClientChannelInfo(channel, addonChannel);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_open_dialog_channel_settings_v1(
            addon, &addonChannel);
      },
      m_clientCapabilities.SupportsChannelSettings());
}

PVR_ERROR CPVRClient::DeleteChannel(const std::shared_ptr<CPVRChannel>& channel)
{
  return DoAddonCall(
      __func__,
      [this, channel](KODI_ADDON_PVR_HDL addon)
      {
        PVR_CHANNEL addonChannel;
        WriteClientChannelInfo(channel, addonChannel);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_delete_channel_v1(addon,
                                                                                 &addonChannel);
      },
      m_clientCapabilities.SupportsChannelSettings());
}

PVR_ERROR CPVRClient::RenameChannel(const std::shared_ptr<CPVRChannel>& channel)
{
  return DoAddonCall(
      __func__,
      [this, channel](KODI_ADDON_PVR_HDL addon)
      {
        PVR_CHANNEL addonChannel;
        WriteClientChannelInfo(channel, addonChannel);
        strncpy(addonChannel.strChannelName, channel->ChannelName().c_str(),
                sizeof(addonChannel.strChannelName) - 1);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_rename_channel_v1(addon,
                                                                                 &addonChannel);
      },
      m_clientCapabilities.SupportsChannelSettings());
}

PVR_ERROR CPVRClient::GetEPGForChannel(int iChannelUid, CPVREpg* epg, time_t start, time_t end)
{
  return DoAddonCall(
      __func__,
      [this, iChannelUid, epg, start, end](KODI_ADDON_PVR_HDL addon)
      {
        int iPVRTimeCorrection =
            CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_iPVRTimeCorrection;

        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_epg_for_channel_v1(
            addon, epg, iChannelUid, start ? start - iPVRTimeCorrection : 0,
            end ? end - iPVRTimeCorrection : 0);
      },
      m_clientCapabilities.SupportsEPG());
}

PVR_ERROR CPVRClient::SetEPGMaxPastDays(int iPastDays)
{
  return DoAddonCall(
      __func__,
      [this, iPastDays](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_set_epg_max_past_days_v1(addon,
                                                                                        iPastDays);
      },
      m_clientCapabilities.SupportsEPG());
}

PVR_ERROR CPVRClient::SetEPGMaxFutureDays(int iFutureDays)
{
  return DoAddonCall(
      __func__,
      [this, iFutureDays](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_set_epg_max_future_days_v1(
            addon, iFutureDays);
      },
      m_clientCapabilities.SupportsEPG());
}

// This class wraps an EPG_TAG (PVR Addon API struct) to ensure that the string members of
// that struct, which are const char pointers, stay valid until the EPG_TAG gets destructed.
// Please note that this struct is also used to transfer huge amount of EPG_TAGs from
// addon to Kodi. Thus, changing the struct to contain char arrays is not recommended,
// because this would lead to huge amount of string copies when transferring epg data
// from addon to Kodi.
class CAddonEpgTag : public EPG_TAG
{
public:
  CAddonEpgTag() = delete;
  explicit CAddonEpgTag(const std::shared_ptr<const CPVREpgInfoTag>& kodiTag)
    : m_strTitle(kodiTag->Title()),
      m_strPlotOutline(kodiTag->PlotOutline()),
      m_strPlot(kodiTag->Plot()),
      m_strOriginalTitle(kodiTag->OriginalTitle()),
      m_strCast(kodiTag->DeTokenize(kodiTag->Cast())),
      m_strDirector(kodiTag->DeTokenize(kodiTag->Directors())),
      m_strWriter(kodiTag->DeTokenize(kodiTag->Writers())),
      m_strIMDBNumber(kodiTag->IMDBNumber()),
      m_strEpisodeName(kodiTag->EpisodeName()),
      m_strIconPath(kodiTag->ClientIconPath()),
      m_strSeriesLink(kodiTag->SeriesLink()),
      m_strGenreDescription(kodiTag->GenreDescription()),
      m_strParentalRatingCode(kodiTag->ParentalRatingCode())
  {
    time_t t;
    kodiTag->StartAsUTC().GetAsTime(t);
    startTime = t;
    kodiTag->EndAsUTC().GetAsTime(t);
    endTime = t;

    const CDateTime firstAired = kodiTag->FirstAired();
    if (firstAired.IsValid())
      m_strFirstAired = firstAired.GetAsW3CDate();

    iUniqueBroadcastId = kodiTag->UniqueBroadcastID();
    iUniqueChannelId = kodiTag->UniqueChannelID();
    iParentalRating = kodiTag->ParentalRating();
    iSeriesNumber = kodiTag->SeriesNumber();
    iEpisodeNumber = kodiTag->EpisodeNumber();
    iEpisodePartNumber = kodiTag->EpisodePart();
    iStarRating = kodiTag->StarRating();
    iYear = kodiTag->Year();
    iFlags = kodiTag->Flags();
    iGenreType = kodiTag->GenreType();
    iGenreSubType = kodiTag->GenreSubType();
    strTitle = m_strTitle.c_str();
    strPlotOutline = m_strPlotOutline.c_str();
    strPlot = m_strPlot.c_str();
    strOriginalTitle = m_strOriginalTitle.c_str();
    strCast = m_strCast.c_str();
    strDirector = m_strDirector.c_str();
    strWriter = m_strWriter.c_str();
    strIMDBNumber = m_strIMDBNumber.c_str();
    strEpisodeName = m_strEpisodeName.c_str();
    strIconPath = m_strIconPath.c_str();
    strSeriesLink = m_strSeriesLink.c_str();
    strGenreDescription = m_strGenreDescription.c_str();
    strFirstAired = m_strFirstAired.c_str();
    strParentalRatingCode = m_strParentalRatingCode.c_str();
  }

  virtual ~CAddonEpgTag() = default;

private:
  std::string m_strTitle;
  std::string m_strPlotOutline;
  std::string m_strPlot;
  std::string m_strOriginalTitle;
  std::string m_strCast;
  std::string m_strDirector;
  std::string m_strWriter;
  std::string m_strIMDBNumber;
  std::string m_strEpisodeName;
  std::string m_strIconPath;
  std::string m_strSeriesLink;
  std::string m_strGenreDescription;
  std::string m_strFirstAired;
  std::string m_strParentalRatingCode;
};

PVR_ERROR CPVRClient::IsRecordable(const std::shared_ptr<const CPVREpgInfoTag>& tag,
                                   bool& bIsRecordable) const
{
  return DoAddonCall(
      __func__,
      [this, tag, &bIsRecordable](KODI_ADDON_PVR_HDL addon)
      {
        CAddonEpgTag addonTag(tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_is_epg_tag_recordable_v1(
            addon, &addonTag, &bIsRecordable);
      },
      m_clientCapabilities.SupportsRecordings() && m_clientCapabilities.SupportsEPG());
}

PVR_ERROR CPVRClient::IsPlayable(const std::shared_ptr<const CPVREpgInfoTag>& tag,
                                 bool& bIsPlayable) const
{
  return DoAddonCall(
      __func__,
      [this, tag, &bIsPlayable](KODI_ADDON_PVR_HDL addon)
      {
        CAddonEpgTag addonTag(tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_is_epg_tag_playable_v1(
            addon, &addonTag, &bIsPlayable);
      },
      m_clientCapabilities.SupportsEPG());
}

void CPVRClient::WriteStreamProperties(const PVR_NAMED_VALUE* properties,
                                       unsigned int iPropertyCount,
                                       CPVRStreamProperties& props)
{
  for (unsigned int i = 0; i < iPropertyCount; ++i)
  {
    props.emplace_back(std::make_pair(properties[i].strName, properties[i].strValue));
  }
}

PVR_ERROR CPVRClient::GetEpgTagStreamProperties(const std::shared_ptr<CPVREpgInfoTag>& tag,
                                                CPVRStreamProperties& props)
{
  return DoAddonCall(
      __func__,
      [this, &tag, &props](KODI_ADDON_PVR_HDL addon)
      {
        CAddonEpgTag addonTag(tag);

        size_t iPropertyCount = STREAM_MAX_PROPERTY_COUNT;
        std::unique_ptr<PVR_NAMED_VALUE[]> properties(new PVR_NAMED_VALUE[iPropertyCount]);
        memset(properties.get(), 0, iPropertyCount * sizeof(PVR_NAMED_VALUE));

        PVR_ERROR error =
            m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_epg_tag_stream_properties_v1(
                addon, &addonTag, properties.get(), &iPropertyCount);
        if (error == PVR_ERROR_NO_ERROR)
          WriteStreamProperties(properties.get(), iPropertyCount, props);

        return error;
      });
}

PVR_ERROR CPVRClient::GetEpgTagEdl(const std::shared_ptr<const CPVREpgInfoTag>& epgTag,
                                   std::vector<PVR_EDL_ENTRY>& edls)
{
  edls.clear();
  return DoAddonCall(
      __func__,
      [this, &epgTag, &edls](KODI_ADDON_PVR_HDL addon)
      {
        CAddonEpgTag addonTag(epgTag);

        PVR_EDL_ENTRY edl_array[PVR_ADDON_EDL_LENGTH];
        size_t size = PVR_ADDON_EDL_LENGTH;
        PVR_ERROR error = m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_epg_tag_edl_v1(
            addon, &addonTag, edl_array, &size);
        if (error == PVR_ERROR_NO_ERROR)
        {
          edls.reserve(size);
          for (size_t i = 0; i < size; ++i)
            edls.emplace_back(edl_array[i]);
        }
        return error;
      },
      m_clientCapabilities.SupportsEpgTagEdl());
}

PVR_ERROR CPVRClient::GetChannelGroupsAmount(int& iGroups)
{
  iGroups = -1;
  return DoAddonCall(
      __func__,
      [this, &iGroups](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_channel_groups_amount_v1(
            addon, &iGroups);
      },
      m_clientCapabilities.SupportsChannelGroups());
}

PVR_ERROR CPVRClient::GetChannelGroups(CPVRChannelGroups* groups)
{
  return DoAddonCall(
      __func__,
      [this, groups](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_channel_groups_v1(
            addon, groups, groups->IsRadio());
      },
      m_clientCapabilities.SupportsChannelGroups());
}

PVR_ERROR CPVRClient::GetChannelGroupMembers(
    CPVRChannelGroup* group, std::vector<std::shared_ptr<CPVRChannelGroupMember>>& groupMembers)
{
  return DoAddonCall(
      __func__,
      [this, group, &groupMembers](KODI_ADDON_PVR_HDL addon)
      {
        PVR_CHANNEL_GROUP tag;
        WriteClientGroupInfo(*group, tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_channel_group_members_v1(
            addon, group, &tag);
      },
      m_clientCapabilities.SupportsChannelGroups());
}

PVR_ERROR CPVRClient::GetProvidersAmount(int& iProviders)
{
  iProviders = -1;
  return DoAddonCall(
      __func__,
      [this, &iProviders](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_providers_amount_v1(addon,
                                                                                       &iProviders);
      });
}

PVR_ERROR CPVRClient::GetProviders(CPVRProvidersContainer& providers)
{
  return DoAddonCall(
      __func__,
      [this, &providers](KODI_ADDON_PVR_HDL addon) {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_providers_v1(addon, &providers);
      },
      m_clientCapabilities.SupportsProviders());
}

PVR_ERROR CPVRClient::GetChannelsAmount(int& iChannels)
{
  iChannels = -1;
  return DoAddonCall(
      __func__,
      [this, &iChannels](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_channels_amount_v1(addon,
                                                                                      &iChannels);
      });
}

PVR_ERROR CPVRClient::GetChannels(bool radio, std::vector<std::shared_ptr<CPVRChannel>>& channels)
{
  return DoAddonCall(
      __func__,
      [this, radio, &channels](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_channels_v1(addon, &channels,
                                                                               radio);
      },
      (radio && m_clientCapabilities.SupportsRadio()) ||
          (!radio && m_clientCapabilities.SupportsTV()));
}

PVR_ERROR CPVRClient::GetRecordingsAmount(bool deleted, int& iRecordings)
{
  iRecordings = -1;
  return DoAddonCall(
      __func__,
      [this, deleted, &iRecordings](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_recordings_amount_v1(
            addon, deleted, &iRecordings);
      },
      m_clientCapabilities.SupportsRecordings() &&
          (!deleted || m_clientCapabilities.SupportsRecordingsUndelete()));
}

PVR_ERROR CPVRClient::GetRecordings(CPVRRecordings* results, bool deleted)
{
  return DoAddonCall(
      __func__,
      [this, results, deleted](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_recordings_v1(addon, results,
                                                                                 deleted);
      },
      m_clientCapabilities.SupportsRecordings() &&
          (!deleted || m_clientCapabilities.SupportsRecordingsUndelete()));
}

PVR_ERROR CPVRClient::DeleteRecording(const CPVRRecording& recording)
{
  return DoAddonCall(
      __func__,
      [this, &recording](KODI_ADDON_PVR_HDL addon)
      {
        PVR_RECORDING tag;
        WriteClientRecordingInfo(recording, tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_delete_recording_v1(addon, &tag);
      },
      m_clientCapabilities.SupportsRecordings() && m_clientCapabilities.SupportsRecordingsDelete());
}

PVR_ERROR CPVRClient::UndeleteRecording(const CPVRRecording& recording)
{
  return DoAddonCall(
      __func__,
      [this, &recording](KODI_ADDON_PVR_HDL addon)
      {
        PVR_RECORDING tag;
        WriteClientRecordingInfo(recording, tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_undelete_recording_v1(addon, &tag);
      },
      m_clientCapabilities.SupportsRecordingsUndelete());
}

PVR_ERROR CPVRClient::DeleteAllRecordingsFromTrash()
{
  return DoAddonCall(
      __func__,
      [this](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_delete_all_recordings_from_trash_v1(
            addon);
      },
      m_clientCapabilities.SupportsRecordingsUndelete());
}

PVR_ERROR CPVRClient::RenameRecording(const CPVRRecording& recording)
{
  return DoAddonCall(
      __func__,
      [this, &recording](KODI_ADDON_PVR_HDL addon)
      {
        PVR_RECORDING tag;
        WriteClientRecordingInfo(recording, tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_rename_recording_v1(addon, &tag);
      },
      m_clientCapabilities.SupportsRecordings());
}

PVR_ERROR CPVRClient::SetRecordingLifetime(const CPVRRecording& recording)
{
  return DoAddonCall(
      __func__,
      [this, &recording](KODI_ADDON_PVR_HDL addon)
      {
        PVR_RECORDING tag;
        WriteClientRecordingInfo(recording, tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_set_recording_lifetime_v1(addon,
                                                                                         &tag);
      },
      m_clientCapabilities.SupportsRecordingsLifetimeChange());
}

PVR_ERROR CPVRClient::SetRecordingPlayCount(const CPVRRecording& recording, int count)
{
  return DoAddonCall(
      __func__,
      [this, &recording, count](KODI_ADDON_PVR_HDL addon)
      {
        PVR_RECORDING tag;
        WriteClientRecordingInfo(recording, tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_set_recording_play_count_v1(
            addon, &tag, count);
      },
      m_clientCapabilities.SupportsRecordingsPlayCount());
}

PVR_ERROR CPVRClient::SetRecordingLastPlayedPosition(const CPVRRecording& recording,
                                                     int lastplayedposition)
{
  return DoAddonCall(
      __func__,
      [this, &recording, lastplayedposition](KODI_ADDON_PVR_HDL addon)
      {
        PVR_RECORDING tag;
        WriteClientRecordingInfo(recording, tag);
        return m_ifc->kodi_addoninstance_pvr_h
            ->kodi_addon_pvr_set_recording_last_played_position_v1(addon, &tag, lastplayedposition);
      },
      m_clientCapabilities.SupportsRecordingsLastPlayedPosition());
}

PVR_ERROR CPVRClient::GetRecordingLastPlayedPosition(const CPVRRecording& recording, int& iPosition)
{
  iPosition = -1;
  return DoAddonCall(
      __func__,
      [this, &recording, &iPosition](KODI_ADDON_PVR_HDL addon)
      {
        PVR_RECORDING tag;
        WriteClientRecordingInfo(recording, tag);
        return m_ifc->kodi_addoninstance_pvr_h
            ->kodi_addon_pvr_get_recording_last_played_position_v1(addon, &tag, &iPosition);
      },
      m_clientCapabilities.SupportsRecordingsLastPlayedPosition());
}

PVR_ERROR CPVRClient::GetRecordingEdl(const CPVRRecording& recording,
                                      std::vector<PVR_EDL_ENTRY>& edls)
{
  edls.clear();
  return DoAddonCall(
      __func__,
      [this, &recording, &edls](KODI_ADDON_PVR_HDL addon)
      {
        PVR_RECORDING tag;
        WriteClientRecordingInfo(recording, tag);

        PVR_EDL_ENTRY edl_array[PVR_ADDON_EDL_LENGTH];
        size_t size = PVR_ADDON_EDL_LENGTH;
        PVR_ERROR error = m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_recording_edl_v1(
            addon, &tag, edl_array, &size);
        if (error == PVR_ERROR_NO_ERROR)
        {
          edls.reserve(size);
          for (size_t i = 0; i < size; ++i)
            edls.emplace_back(edl_array[i]);
        }
        return error;
      },
      m_clientCapabilities.SupportsRecordingsEdl());
}

PVR_ERROR CPVRClient::GetRecordingSize(const CPVRRecording& recording, int64_t& sizeInBytes)
{
  return DoAddonCall(
      __func__,
      [this, &recording, &sizeInBytes](KODI_ADDON_PVR_HDL addon)
      {
        PVR_RECORDING tag;
        WriteClientRecordingInfo(recording, tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_recording_size_v1(addon, &tag,
                                                                                     &sizeInBytes);
      },
      m_clientCapabilities.SupportsRecordingsSize());
}

PVR_ERROR CPVRClient::GetTimersAmount(int& iTimers)
{
  iTimers = -1;
  return DoAddonCall(
      __func__,
      [this, &iTimers](KODI_ADDON_PVR_HDL addon) {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_timers_amount_v1(addon,
                                                                                    &iTimers);
      },
      m_clientCapabilities.SupportsTimers());
}

PVR_ERROR CPVRClient::GetTimers(CPVRTimersContainer* results)
{
  return DoAddonCall(
      __func__,
      [this, results](KODI_ADDON_PVR_HDL addon)
      { return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_timers_v1(addon, results); },
      m_clientCapabilities.SupportsTimers());
}

PVR_ERROR CPVRClient::AddTimer(const CPVRTimerInfoTag& timer)
{
  return DoAddonCall(
      __func__,
      [this, &timer](KODI_ADDON_PVR_HDL addon)
      {
        PVR_TIMER tag;
        WriteClientTimerInfo(timer, tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_add_timer_v1(addon, &tag);
      },
      m_clientCapabilities.SupportsTimers());
}

PVR_ERROR CPVRClient::DeleteTimer(const CPVRTimerInfoTag& timer, bool bForce /* = false */)
{
  return DoAddonCall(
      __func__,
      [this, &timer, bForce](KODI_ADDON_PVR_HDL addon)
      {
        PVR_TIMER tag;
        WriteClientTimerInfo(timer, tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_delete_timer_v1(addon, &tag, bForce);
      },
      m_clientCapabilities.SupportsTimers());
}

PVR_ERROR CPVRClient::UpdateTimer(const CPVRTimerInfoTag& timer)
{
  return DoAddonCall(
      __func__,
      [this, &timer](KODI_ADDON_PVR_HDL addon)
      {
        PVR_TIMER tag;
        WriteClientTimerInfo(timer, tag);
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_update_timer_v1(addon, &tag);
      },
      m_clientCapabilities.SupportsTimers());
}

PVR_ERROR CPVRClient::GetTimerTypes(std::vector<std::shared_ptr<CPVRTimerType>>& results) const
{
  std::unique_lock<CCriticalSection> lock(m_critSection);
  results = m_timertypes;
  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR CPVRClient::SignalQuality(int channelUid, PVR_SIGNAL_STATUS& qualityinfo)
{
  return DoAddonCall(__func__,
                     [this, channelUid, &qualityinfo](KODI_ADDON_PVR_HDL addon)
                     {
                       return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_signal_status_v1(
                           addon, channelUid, &qualityinfo);
                     });
}

PVR_ERROR CPVRClient::GetDescrambleInfo(int channelUid, PVR_DESCRAMBLE_INFO& descrambleinfo) const
{
  return DoAddonCall(
      __func__,
      [this, channelUid, &descrambleinfo](KODI_ADDON_PVR_HDL addon)
      {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_descramble_info_v1(
            addon, channelUid, &descrambleinfo);
      },
      m_clientCapabilities.SupportsDescrambleInfo());
}

PVR_ERROR CPVRClient::GetChannelStreamProperties(const std::shared_ptr<CPVRChannel>& channel,
                                                 CPVRStreamProperties& props)
{
  return DoAddonCall(
      __func__,
      [this, &channel, &props](KODI_ADDON_PVR_HDL addon)
      {
        if (!CanPlayChannel(channel))
          return PVR_ERROR_NO_ERROR; // no error, but no need to obtain the values from the addon

        PVR_CHANNEL tag = {};
        WriteClientChannelInfo(channel, tag);

        size_t iPropertyCount = STREAM_MAX_PROPERTY_COUNT;
        std::unique_ptr<PVR_NAMED_VALUE[]> properties(new PVR_NAMED_VALUE[iPropertyCount]);
        memset(properties.get(), 0, iPropertyCount * sizeof(PVR_NAMED_VALUE));

        PVR_ERROR error =
            m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_channel_stream_properties_v1(
                addon, &tag, properties.get(), &iPropertyCount);
        if (error == PVR_ERROR_NO_ERROR)
          WriteStreamProperties(properties.get(), iPropertyCount, props);

        return error;
      });
}

PVR_ERROR CPVRClient::GetRecordingStreamProperties(const std::shared_ptr<CPVRRecording>& recording,
                                                   CPVRStreamProperties& props)
{
  return DoAddonCall(
      __func__,
      [this, &recording, &props](KODI_ADDON_PVR_HDL addon)
      {
        if (!m_clientCapabilities.SupportsRecordings())
          return PVR_ERROR_NO_ERROR; // no error, but no need to obtain the values from the addon

        PVR_RECORDING tag = {};
        WriteClientRecordingInfo(*recording, tag);

        size_t iPropertyCount = STREAM_MAX_PROPERTY_COUNT;
        std::unique_ptr<PVR_NAMED_VALUE[]> properties(new PVR_NAMED_VALUE[iPropertyCount]);
        memset(properties.get(), 0, iPropertyCount * sizeof(PVR_NAMED_VALUE));

        PVR_ERROR error =
            m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_get_recording_stream_properties_v1(
                addon, &tag, properties.get(), &iPropertyCount);
        if (error == PVR_ERROR_NO_ERROR)
          WriteStreamProperties(properties.get(), iPropertyCount, props);

        return error;
      });
}

const char* CPVRClient::ToString(const PVR_ERROR error)
{
  switch (error)
  {
    case PVR_ERROR_NO_ERROR:
      return "no error";
    case PVR_ERROR_NOT_IMPLEMENTED:
      return "not implemented";
    case PVR_ERROR_SERVER_ERROR:
      return "server error";
    case PVR_ERROR_SERVER_TIMEOUT:
      return "server timeout";
    case PVR_ERROR_RECORDING_RUNNING:
      return "recording already running";
    case PVR_ERROR_ALREADY_PRESENT:
      return "already present";
    case PVR_ERROR_REJECTED:
      return "rejected by the backend";
    case PVR_ERROR_INVALID_PARAMETERS:
      return "invalid parameters for this method";
    case PVR_ERROR_FAILED:
      return "the command failed";
    case PVR_ERROR_UNKNOWN:
    default:
      return "unknown error";
  }
}

PVR_ERROR CPVRClient::DoAddonCall(
    const char* strFunctionName,
    const std::function<PVR_ERROR(const KODI_ADDON_PVR_HDL)>& function,
    bool bIsImplemented /* = true */,
    bool bCheckReadyToUse /* = true */) const
{
  // Check preconditions.
  if (!bIsImplemented)
    return PVR_ERROR_NOT_IMPLEMENTED;

  if (m_bBlockAddonCalls)
  {
    CLog::Log(LOGWARNING, "{}: Blocking call to add-on '{}'.", strFunctionName, ID());
    return PVR_ERROR_SERVER_ERROR;
  }

  if (bCheckReadyToUse && IgnoreClient())
  {
    CLog::Log(LOGWARNING, "{}: Blocking call to add-on '{}'. Add-on not (yet) connected.",
              strFunctionName, ID());
    return PVR_ERROR_SERVER_ERROR;
  }

  if (bCheckReadyToUse && !ReadyToUse())
  {
    CLog::Log(LOGWARNING, "{}: Blocking call to add-on '{}'. Add-on not ready to use.",
              strFunctionName, ID());
    return PVR_ERROR_SERVER_ERROR;
  }

  // Call.
  m_allAddonCallsFinished.Reset();
  m_iAddonCalls++;

  const PVR_ERROR error = function(m_instance);

  m_iAddonCalls--;
  if (m_iAddonCalls == 0)
    m_allAddonCallsFinished.Set();

  // Log error, if any.
  if (error != PVR_ERROR_NO_ERROR && error != PVR_ERROR_NOT_IMPLEMENTED)
    CLog::Log(LOGERROR, "{}: Add-on '{}' returned an error: {}", strFunctionName, ID(),
              ToString(error));

  return error;
}

bool CPVRClient::CanPlayChannel(const std::shared_ptr<CPVRChannel>& channel) const
{
  return (m_bReadyToUse && ((m_clientCapabilities.SupportsTV() && !channel->IsRadio()) ||
                            (m_clientCapabilities.SupportsRadio() && channel->IsRadio())));
}

PVR_ERROR CPVRClient::OnSystemSleep()
{
  return DoAddonCall(
      __func__, [this](KODI_ADDON_PVR_HDL addon)
      { return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_on_system_sleep_v1(addon); });
}

PVR_ERROR CPVRClient::OnSystemWake()
{
  return DoAddonCall(
      __func__, [this](KODI_ADDON_PVR_HDL addon)
      { return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_on_system_wake_v1(addon); });
}

PVR_ERROR CPVRClient::OnPowerSavingActivated()
{
  return DoAddonCall(
      __func__,
      [this](KODI_ADDON_PVR_HDL addon) {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_on_power_saving_activated_v1(addon);
      });
}

PVR_ERROR CPVRClient::OnPowerSavingDeactivated()
{
  return DoAddonCall(
      __func__,
      [this](KODI_ADDON_PVR_HDL addon) {
        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_on_power_saving_deactivated_v1(
            addon);
      });
}

std::shared_ptr<CPVRClientMenuHooks> CPVRClient::GetMenuHooks()
{
  if (!m_menuhooks)
    m_menuhooks.reset(new CPVRClientMenuHooks(ID()));

  return m_menuhooks;
}

PVR_ERROR CPVRClient::CallEpgTagMenuHook(const CPVRClientMenuHook& hook,
                                         const std::shared_ptr<CPVREpgInfoTag>& tag)
{
  return DoAddonCall(__func__,
                     [this, &hook, &tag](KODI_ADDON_PVR_HDL addon)
                     {
                       CAddonEpgTag addonTag(tag);

                       PVR_MENUHOOK menuHook;
                       menuHook.category = PVR_MENUHOOK_EPG;
                       menuHook.iHookId = hook.GetId();
                       menuHook.iLocalizedStringId = hook.GetLabelId();

                       return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_call_epg_menu_hook_v1(
                           addon, &menuHook, &addonTag);
                     });
}

PVR_ERROR CPVRClient::CallChannelMenuHook(const CPVRClientMenuHook& hook,
                                          const std::shared_ptr<CPVRChannel>& channel)
{
  return DoAddonCall(
      __func__,
      [this, &hook, &channel](KODI_ADDON_PVR_HDL addon)
      {
        PVR_CHANNEL tag;
        WriteClientChannelInfo(channel, tag);

        PVR_MENUHOOK menuHook;
        menuHook.category = PVR_MENUHOOK_CHANNEL;
        menuHook.iHookId = hook.GetId();
        menuHook.iLocalizedStringId = hook.GetLabelId();

        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_call_channel_menu_hook_v1(
            addon, &menuHook, &tag);
      });
}

PVR_ERROR CPVRClient::CallRecordingMenuHook(const CPVRClientMenuHook& hook,
                                            const std::shared_ptr<CPVRRecording>& recording,
                                            bool bDeleted)
{
  return DoAddonCall(
      __func__,
      [this, &hook, &recording, &bDeleted](KODI_ADDON_PVR_HDL addon)
      {
        PVR_RECORDING tag;
        WriteClientRecordingInfo(*recording, tag);

        PVR_MENUHOOK menuHook;
        menuHook.category = bDeleted ? PVR_MENUHOOK_DELETED_RECORDING : PVR_MENUHOOK_RECORDING;
        menuHook.iHookId = hook.GetId();
        menuHook.iLocalizedStringId = hook.GetLabelId();

        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_call_recording_menu_hook_v1(
            addon, &menuHook, &tag);
      });
}

PVR_ERROR CPVRClient::CallTimerMenuHook(const CPVRClientMenuHook& hook,
                                        const std::shared_ptr<CPVRTimerInfoTag>& timer)
{
  return DoAddonCall(
      __func__,
      [this, &hook, &timer](KODI_ADDON_PVR_HDL addon)
      {
        PVR_TIMER tag;
        WriteClientTimerInfo(*timer, tag);

        PVR_MENUHOOK menuHook;
        menuHook.category = PVR_MENUHOOK_TIMER;
        menuHook.iHookId = hook.GetId();
        menuHook.iLocalizedStringId = hook.GetLabelId();

        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_call_timer_menu_hook_v1(
            addon, &menuHook, &tag);
      });
}

PVR_ERROR CPVRClient::CallSettingsMenuHook(const CPVRClientMenuHook& hook)
{
  return DoAddonCall(
      __func__,
      [this, &hook](KODI_ADDON_PVR_HDL addon)
      {
        PVR_MENUHOOK menuHook;
        menuHook.category = PVR_MENUHOOK_SETTING;
        menuHook.iHookId = hook.GetId();
        menuHook.iLocalizedStringId = hook.GetLabelId();

        return m_ifc->kodi_addoninstance_pvr_h->kodi_addon_pvr_call_settings_menu_hook_v1(
            addon, &menuHook);
      });
}

void CPVRClient::SetPriority(int iPriority)
{
  std::unique_lock<CCriticalSection> lock(m_critSection);
  if (m_iPriority != iPriority)
  {
    m_iPriority = iPriority;
    if (m_iClientId > PVR_INVALID_CLIENT_ID)
    {
      CServiceBroker::GetPVRManager().GetTVDatabase()->Persist(*this);
      m_bPriorityFetched = true;
    }
  }
}

int CPVRClient::GetPriority() const
{
  std::unique_lock<CCriticalSection> lock(m_critSection);
  if (!m_bPriorityFetched && m_iClientId > PVR_INVALID_CLIENT_ID)
  {
    m_iPriority = CServiceBroker::GetPVRManager().GetTVDatabase()->GetPriority(*this);
    m_bPriorityFetched = true;
  }
  return m_iPriority;
}

void CPVRClient::HandleAddonCallback(const char* strFunctionName,
                                     const std::function<void()>& function,
                                     bool bForceCall /* = false */)
{
  // Check preconditions.
  if (!bForceCall && m_bBlockAddonCalls && m_iAddonCalls == 0)
  {
    CLog::Log(LOGWARNING, LOGPVR, "{}: Ignoring callback from PVR client '{}'", strFunctionName,
              ID());
    return;
  }

  // Call.
  function();
}

int CPVRClient::cb_get_epg_max_future_days()
{
  int result = -1;
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        // update channels in the next iteration of the pvrmanager's main loop
                        result =
                            CServiceBroker::GetPVRManager().EpgContainer().GetFutureDaysToDisplay();
                      });
  return result;
}

int CPVRClient::cb_get_epg_max_past_days()
{
  int result = -1;
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        // update channels in the next iteration of the pvrmanager's main loop
                        result =
                            CServiceBroker::GetPVRManager().EpgContainer().GetPastDaysToDisplay();
                      });
  return result;
}

void CPVRClient::cb_transfer_channel_group(const KODI_ADDON_PVR_TRANSFER_HDL handle,
                                           const PVR_CHANNEL_GROUP* group)
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        if (!handle || !group)
                        {
                          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
                          return;
                        }

                        if (strlen(group->strGroupName) == 0)
                        {
                          CLog::LogF(LOGERROR, "Empty group name");
                          return;
                        }

                        // transfer this entry to the groups container
                        CPVRChannelGroups* kodiGroups = static_cast<CPVRChannelGroups*>(handle);
                        const auto transferGroup =
                            std::make_shared<CPVRChannelGroup>(*group, kodiGroups->GetGroupAll());
                        kodiGroups->UpdateFromClient(transferGroup);
                      });
}

void CPVRClient::cb_transfer_channel_group_member(const KODI_ADDON_PVR_TRANSFER_HDL handle,
                                                  const PVR_CHANNEL_GROUP_MEMBER* member)
{
  HandleAddonCallback(
      __func__,
      [&]()
      {
        if (!handle || !member)
        {
          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
          return;
        }

        const std::shared_ptr<CPVRChannel> channel =
            CServiceBroker::GetPVRManager().ChannelGroups()->GetByUniqueID(member->iChannelUniqueId,
                                                                           GetID());
        if (!channel)
        {
          CLog::LogF(LOGERROR, "Cannot find group '{}' or channel '{}'", member->strGroupName,
                     member->iChannelUniqueId);
        }
        else
        {
          auto* groupMembers =
              static_cast<std::vector<std::shared_ptr<CPVRChannelGroupMember>>*>(handle);
          groupMembers->emplace_back(
              std::make_shared<CPVRChannelGroupMember>(-1, // real id will be added later
                                                       member->strGroupName, channel));
        }
      });
}

void CPVRClient::cb_transfer_epg_entry(const KODI_ADDON_PVR_TRANSFER_HDL handle,
                                       const EPG_TAG* epgentry)
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        if (!handle || !epgentry)
                        {
                          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
                          return;
                        }

                        // transfer this entry to the epg
                        CPVREpg* epg = static_cast<CPVREpg*>(handle);
                        epg->UpdateEntry(epgentry, GetID());
                      });
}

void CPVRClient::cb_transfer_provider_entry(const KODI_ADDON_PVR_TRANSFER_HDL handle,
                                            const PVR_PROVIDER* provider)
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        if (!handle || !provider)
                        {
                          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
                          return;
                        }

                        /* transfer this entry to the internal channels group */
                        std::shared_ptr<CPVRProvider> transferProvider(
                            std::make_shared<CPVRProvider>(*provider, GetID()));
                        CPVRProvidersContainer* kodiProviders =
                            static_cast<CPVRProvidersContainer*>(handle);
                        kodiProviders->UpdateFromClient(transferProvider);
                      });
}

void CPVRClient::cb_transfer_channel_entry(const KODI_ADDON_PVR_TRANSFER_HDL handle,
                                           const PVR_CHANNEL* channel)
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        if (!handle || !channel)
                        {
                          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
                          return;
                        }

                        auto* channels =
                            static_cast<std::vector<std::shared_ptr<CPVRChannel>>*>(handle);
                        channels->emplace_back(std::make_shared<CPVRChannel>(*channel, GetID()));
                      });
}

void CPVRClient::cb_transfer_recording_entry(const KODI_ADDON_PVR_TRANSFER_HDL handle,
                                             const PVR_RECORDING* recording)
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        if (!handle || !recording)
                        {
                          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
                          return;
                        }

                        // transfer this entry to the recordings container
                        const std::shared_ptr<CPVRRecording> transferRecording =
                            std::make_shared<CPVRRecording>(*recording, GetID());
                        CPVRRecordings* recordings = static_cast<CPVRRecordings*>(handle);
                        recordings->UpdateFromClient(transferRecording, *this);
                      });
}

void CPVRClient::cb_transfer_timer_entry(const KODI_ADDON_PVR_TRANSFER_HDL handle,
                                         const PVR_TIMER* timer)
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        if (!handle || !timer)
                        {
                          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
                          return;
                        }

                        // Note: channel can be nullptr here, for instance for epg-based timer rules
                        //       ("record on any channel" condition)
                        const std::shared_ptr<CPVRChannel> channel =
                            CServiceBroker::GetPVRManager().ChannelGroups()->GetByUniqueID(
                                timer->iClientChannelUid, GetID());

                        // transfer this entry to the timers container
                        const std::shared_ptr<CPVRTimerInfoTag> transferTimer =
                            std::make_shared<CPVRTimerInfoTag>(*timer, channel, GetID());
                        CPVRTimersContainer* timers = static_cast<CPVRTimersContainer*>(handle);
                        timers->UpdateFromClient(transferTimer);
                      });
}

void CPVRClient::cb_add_menu_hook(const PVR_MENUHOOK* hook)
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        if (!hook)
                        {
                          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
                          return;
                        }

                        GetMenuHooks()->AddHook(*hook);
                      });
}

void CPVRClient::cb_recording_notification(const char* strName,
                                           const char* strFileName,
                                           bool bOnOff)
{
  HandleAddonCallback(
      __func__,
      [&]()
      {
        if (!strFileName)
        {
          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
          return;
        }

        const std::string strLine1 =
            StringUtils::Format(g_localizeStrings.Get(bOnOff ? 19197 : 19198), Name());
        std::string strLine2;
        if (strName)
          strLine2 = strName;
        else if (strFileName)
          strLine2 = strFileName;

        // display a notification for 5 seconds
        CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info, strLine1, strLine2, 5000,
                                              false);
        auto eventLog = CServiceBroker::GetEventLog();
        if (eventLog)
          eventLog->Add(EventPtr(new CNotificationEvent(Name(), strLine1, Icon(), strLine2)));

        CLog::LogFC(LOGDEBUG, LOGPVR, "Recording {} on client '{}'. name='{}' filename='{}'",
                    bOnOff ? "started" : "finished", ID(), strName, strFileName);
      });
}

void CPVRClient::cb_trigger_channel_update()
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        // update channels in the next iteration of the pvrmanager's main loop
                        CServiceBroker::GetPVRManager().TriggerChannelsUpdate(GetID());
                      });
}

void CPVRClient::cb_trigger_provider_update()
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        /* update the providers table in the next iteration of the pvrmanager's main loop */
                        CServiceBroker::GetPVRManager().TriggerProvidersUpdate(GetID());
                      });
}

void CPVRClient::cb_trigger_timer_update()
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        // update timers in the next iteration of the pvrmanager's main loop
                        CServiceBroker::GetPVRManager().TriggerTimersUpdate(GetID());
                      });
}

void CPVRClient::cb_trigger_recording_update()
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        // update recordings in the next iteration of the pvrmanager's main loop
                        CServiceBroker::GetPVRManager().TriggerRecordingsUpdate(GetID());
                      });
}

void CPVRClient::cb_trigger_channel_groups_update()
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        // update all channel groups in the next iteration of the pvrmanager's main loop
                        CServiceBroker::GetPVRManager().TriggerChannelGroupsUpdate(GetID());
                      });
}

void CPVRClient::cb_trigger_epg_update(unsigned int iChannelUid)
{
  HandleAddonCallback(
      __func__, [&]()
      { CServiceBroker::GetPVRManager().EpgContainer().UpdateRequest(GetID(), iChannelUid); });
}

void CPVRClient::cb_connection_state_change(const char* strConnectionString,
                                            PVR_CONNECTION_STATE newState,
                                            const char* strMessage)
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        if (!strConnectionString)
                        {
                          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
                          return;
                        }

                        const PVR_CONNECTION_STATE prevState(GetConnectionState());
                        if (prevState == newState)
                          return;

                        CLog::LogFC(
                            LOGDEBUG, LOGPVR,
                            "State for connection '{}' on client '{}' changed from '{}' to '{}'",
                            strConnectionString, ID(), prevState, newState);

                        SetConnectionState(newState);

                        std::string msg;
                        if (strMessage)
                          msg = strMessage;

                        CServiceBroker::GetPVRManager().ConnectionStateChange(
                            this, std::string(strConnectionString), newState, msg);
                      });
}

void CPVRClient::cb_epg_event_state_change(EPG_TAG* tag, EPG_EVENT_STATE newState)
{
  HandleAddonCallback(__func__,
                      [&]()
                      {
                        if (!tag)
                        {
                          CLog::LogF(LOGERROR, "Invalid callback parameter(s)");
                          return;
                        }

                        // Note: channel data and epg id may not yet be available. Tag will be fully initialized later.
                        const std::shared_ptr<CPVREpgInfoTag> epgTag =
                            std::make_shared<CPVREpgInfoTag>(*tag, GetID(), nullptr, -1);
                        CServiceBroker::GetPVRManager().EpgContainer().UpdateFromClient(epgTag,
                                                                                        newState);
                      });
}

} // namespace PVR

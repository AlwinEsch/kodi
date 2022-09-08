/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "AddonType.h"

#include "addons/addoninfo/AddonInfo.h"
#include "utils/URIUtils.h"

namespace ADDON
{
static const std::set<TYPE> dependencyTypes = {
    ADDON_SCRAPER_LIBRARY,
    ADDON_SCRIPT_LIBRARY,
    ADDON_SCRIPT_MODULE,
};

typedef struct
{
  AddonLanguage language;
  std::string name;
  int localizedStringID;
} LanguageMapping;

// clang-format off
static const LanguageMapping languages[] =
  {{ AddonLanguage::Unknown,    "unknown",    2300 },
   { AddonLanguage::C,          "C",          2301 },
   { AddonLanguage::CPP,        "C++",        2302 },
   { AddonLanguage::Groovy,     "Groovy",     2303 },
   { AddonLanguage::Java,       "Java",       2304 },
   { AddonLanguage::JavaScript, "JavaScript", 2305 },
   { AddonLanguage::Python3,    "Python3",    2306 },
   { AddonLanguage::Ruby,       "Ruby",       2307 },
   { AddonLanguage::Skin,       "Skin",       2308 },
   { AddonLanguage::XML,        "XML",        2309 }
  };
// clang-format on

} /* namespace ADDON */

using namespace ADDON;

std::string CAddonType::LibPath() const
{
  if (m_libname.empty())
    return "";
  return URIUtils::AddFileToFolder(m_path, m_libname);
}

const std::string& CAddonType::LanguageName() const
{
  for (const LanguageMapping& map : languages)
  {
    if (map.language == m_language)
      return map.name;
  }
  return languages[0].name;
}

int CAddonType::LanguageLocalizedStringID() const
{
  for (const LanguageMapping& map : languages)
  {
    if (map.language == m_language)
      return map.localizedStringID;
  }
  return languages[0].localizedStringID;
}

void CAddonType::SetType(TYPE type)
{
  m_type = type;
}

void CAddonType::SetProvides(const std::string& content)
{
  if (!content.empty())
  {
    /*
     * Normally the "provides" becomes added from xml scan, but for add-ons
     * stored in the database (e.g. repository contents) it might not be
     * available. Since this information is available in add-on metadata for the
     * main type (see extrainfo) we take the function contents and insert it if
     * empty.
     */
    if (GetValue("provides").empty())
      Insert("provides", content);

    for (const auto& provide : StringUtils::Split(content, ' '))
    {
      TYPE content = CAddonInfo::TranslateSubContent(provide);
      if (content != ADDON_UNKNOWN)
        m_providedSubContent.insert(content);
    }
  }
}

void CAddonType::SetLanguage(const std::string& name)
{
  for (const LanguageMapping& map : languages)
  {
    if (StringUtils::EqualsNoCase(map.name, name))
    {
      m_language = map.language;
      return;
    }
  }
  m_language = AddonLanguage::Unknown;
}

void CAddonType::SetLanguage(AddonLanguage lang)
{
  m_language = lang;
}

bool CAddonType::IsDependencyType(TYPE type)
{
  return dependencyTypes.find(type) != dependencyTypes.end();
}

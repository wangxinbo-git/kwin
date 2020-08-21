/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2015 Mika Allan Rauhala <mika.allan.rauhala@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "decorationpalette.h"
#include "decorations_logging.h"

#include <KConfigGroup>
#include <KSharedConfig>
#include <KColorScheme>

#include <QPalette>
#include <QFileInfo>
#include <QStandardPaths>

namespace KWin
{
namespace Decoration
{

DecorationPalette::DecorationPalette(const QString &colorScheme)
    : m_colorScheme(QFileInfo(colorScheme).isAbsolute()
                    ? colorScheme
                    : QStandardPaths::locate(QStandardPaths::GenericConfigLocation, colorScheme))
{
    if (!m_colorScheme.startsWith(QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)) && colorScheme == QStringLiteral("kdeglobals")) {
        // kdeglobals doesn't exist so create it. This is needed to monitor it using QFileSystemWatcher.
        auto config = KSharedConfig::openConfig(colorScheme, KConfig::SimpleConfig);
        KConfigGroup wmConfig(config, QStringLiteral("WM"));
        wmConfig.writeEntry("FakeEntryToKeepThisGroup", true);
        config->sync();

        m_colorScheme = QStandardPaths::locate(QStandardPaths::GenericConfigLocation, colorScheme);
    }
    m_watcher.addPath(m_colorScheme);
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, [this]() {
        m_watcher.addPath(m_colorScheme);
        update();
        emit changed();
    });

    update();
}

bool DecorationPalette::isValid() const
{
    return true;
}

QColor DecorationPalette::color(KDecoration2::ColorGroup group, KDecoration2::ColorRole role) const
{
    using KDecoration2::ColorRole;
    using KDecoration2::ColorGroup;

    if (m_useLegacy) {
        switch (role) {
            case ColorRole::Frame:
                switch (group) {
                    case ColorGroup::Active:
                        return m_legacyPalette.activeFrameColor;
                    case ColorGroup::Inactive:
                        return m_legacyPalette.inactiveFrameColor;
                    default:
                        return QColor();
                }
            case ColorRole::TitleBar:
                switch (group) {
                    case ColorGroup::Active:
                        return m_legacyPalette.activeTitleBarColor;
                    case ColorGroup::Inactive:
                        return m_legacyPalette.inactiveTitleBarColor;
                    default:
                        return QColor();
                }
            case ColorRole::Foreground:
                switch (group) {
                    case ColorGroup::Active:
                        return m_legacyPalette.activeForegroundColor;
                    case ColorGroup::Inactive:
                        return m_legacyPalette.inactiveForegroundColor;
                    case ColorGroup::Warning:
                        return m_legacyPalette.warningForegroundColor;
                    default:
                        return QColor();
                }
            default:
                return QColor();
        }
    }

    switch (role) {
        case ColorRole::Frame:
            switch (group) {
                case ColorGroup::Active:
                    return KColorScheme(QPalette::Normal, KColorScheme::ColorSet::Header, m_colorSchemeConfig).shade(KColorScheme::ShadeRole::ShadowShade);
                case ColorGroup::Inactive:
                    return KColorScheme(QPalette::Inactive, KColorScheme::ColorSet::Header, m_colorSchemeConfig).shade(KColorScheme::ShadeRole::ShadowShade);
                default:
                    return QColor();
            }
        case ColorRole::TitleBar:
            switch (group) {
                case ColorGroup::Active:
                    return KColorScheme(QPalette::Normal, KColorScheme::ColorSet::Header, m_colorSchemeConfig).background().color();
                case ColorGroup::Inactive:
                    return KColorScheme(QPalette::Normal, KColorScheme::ColorSet::Header, m_colorSchemeConfig).background().color();
                default:
                    return QColor();
            }
        case ColorRole::Foreground:
            switch (group) {
                case ColorGroup::Active:
                    return KColorScheme(QPalette::Normal, KColorScheme::ColorSet::Header, m_colorSchemeConfig).foreground().color();
                case ColorGroup::Inactive:
                    return KColorScheme(QPalette::Inactive, KColorScheme::ColorSet::Header, m_colorSchemeConfig).foreground().color();
                case ColorGroup::Warning:
                    return KColorScheme(QPalette::Inactive, KColorScheme::ColorSet::Header, m_colorSchemeConfig).foreground(KColorScheme::ForegroundRole::NegativeText).color();
                default:
                    return QColor();
            }
        default:
            return QColor();
    }
}

QPalette DecorationPalette::palette() const
{
    return m_palette;
}

void DecorationPalette::update()
{
    m_colorSchemeConfig = KSharedConfig::openConfig(m_colorScheme, KConfig::SimpleConfig);
    if (m_colorSchemeConfig->group("Colors").hasGroup("Header")) {
        auto config = KSharedConfig::openConfig(m_colorScheme, KConfig::SimpleConfig);
        KConfigGroup wmConfig(config, QStringLiteral("WM"));

        if (!wmConfig.exists() && !m_colorScheme.endsWith(QStringLiteral("/kdeglobals"))) {
            qCWarning(KWIN_DECORATIONS) << "Invalid color scheme" << m_colorScheme << "lacks WM group";
            return;
        }

        m_palette = KColorScheme::createApplicationPalette(config);

        m_legacyPalette.activeFrameColor        = wmConfig.readEntry("frame", m_palette.color(QPalette::Active, QPalette::Window));
        m_legacyPalette.inactiveFrameColor      = wmConfig.readEntry("inactiveFrame", m_legacyPalette.activeFrameColor);
        m_legacyPalette.activeTitleBarColor     = wmConfig.readEntry("activeBackground", m_palette.color(QPalette::Active, QPalette::Highlight));
        m_legacyPalette.inactiveTitleBarColor   = wmConfig.readEntry("inactiveBackground", m_legacyPalette.inactiveTitleBarColor);
        m_legacyPalette.activeForegroundColor   = wmConfig.readEntry("activeForeground", m_palette.color(QPalette::Active, QPalette::HighlightedText));
        m_legacyPalette.inactiveForegroundColor = wmConfig.readEntry("inactiveForeground", m_legacyPalette.activeForegroundColor.darker());

        KConfigGroup windowColorsConfig(config, QStringLiteral("Colors:Window"));
        m_legacyPalette.warningForegroundColor = windowColorsConfig.readEntry("ForegroundNegative", QColor(237, 21, 2));
        m_useLegacy = true;
    } else {
        m_palette = KColorScheme::createApplicationPalette(m_colorSchemeConfig);
        m_useLegacy = false;
    }
}

}
}

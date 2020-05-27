/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2014 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2014 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2015 Mika Allan Rauhala <mika.allan.rauhala@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KWIN_DECORATION_PALETTE_H
#define KWIN_DECORATION_PALETTE_H

#include <KDecoration2/DecorationSettings>
#include <QFileSystemWatcher>
#include <QPalette>
#include <KSharedConfig>

namespace KWin
{
namespace Decoration
{

class DecorationPalette : public QObject
{
    Q_OBJECT
public:
    DecorationPalette(const QString &colorScheme);

    bool isValid() const;

    QColor color(KDecoration2::ColorGroup group, KDecoration2::ColorRole role) const;
    QPalette palette() const;

Q_SIGNALS:
    void changed();
private:
    void update();

    QString m_colorScheme;
    QFileSystemWatcher m_watcher;

    KSharedConfig::Ptr m_colorSchemeConfig;

    QPalette m_palette;
};

}
}

#endif

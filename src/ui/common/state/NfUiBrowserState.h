/**
 * File name: NfBrowserState.h
 * Project: Neofluxon (a photography workflow software)
 *
 * Copyright (C) 2026 Iurie Nistor
 *
 * This file is part of Neofluxon.
 *
 * Neofluxon is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef NF_UI_BROWSER_STATE_H
#define NF_UI_BROWSER_STATE_H

#include "core/NfPhoto.h"

#include <QObject>
#include <unordered_set>
#include <set>

namespace NfUi {

class NfUiBrowserState : public QObject
{
        Q_OBJECT

public:
        using PhotoFormatFilter = std::set<NfCore::NfPhoto::PhotoFormat>;
        enum class ViewMode {
                Grid,
                Preview
        };

        enum class SortField {
                Name,
                DateTaken,
                FileSize
        };
        using SortFields = std::unordered_set<SortField>;

        enum class SortOrder {
                Ascending,
                Descending
        };

        explicit NfUiBrowserState(QObject* parent = nullptr);
        ViewMode viewMode() const;
        void setCurrentPhoto(const NfCore::NfPhoto& photo);
        const NfCore::NfPhoto& currentPhoto() const;
        const PhotoFormatFilter& formatFilter() const;
        const SortFields& sortingFields() const;
        SortOrder sortOrder() const;

signals:
        void viewModeChanged(ViewMode mode);
        void currentPhotoChanged(const NfCore::NfPhoto& photo);
        void formatFilterChanged(const PhotoFormatFilter& formats);
        void sortingUpdated();

public slots:
        void setViewMode(ViewMode mode);
        void setFormatFilter(const PhotoFormatFilter& formats);
        void setSortFields(const SortFields &fields);
        void enableSortField(SortField field, bool b);
        void setSortOrder(SortOrder sortOrder);

private:
        ViewMode m_viewMode;
        NfCore::NfPhoto m_photo;
        PhotoFormatFilter m_formatFilter;
        SortFields m_sortFields;
        SortOrder m_sortOrder{};
};
} // namespace NfUi

#endif // NF_UI_BROWSER_STATE_H

#ifndef NF_LIBRARY_PHOTO_SOURCE_H
#define NF_LIBRARY_PHOTO_SOURCE_H

#include "NfPhotoSource.h"

namespace NfCore {

class NfPhotoScanner;
class NfLibraryQuery;

class NfLibraryPhotoSource : public NfPhotoSource
{
public:
    NfLibraryPhotoSource() = default;
    explicit NfLibraryPhotoSource(const NfLibraryQuery& query);

    void setQuery(const NfLibraryQuery& q);
    const NfLibraryQuery& query() const;

    void apply(NfPhotoScanner& scanner) const override;

private:
    NfLibraryQuery m_query;
};

} // namespace NfCore

#endif // NF_LIBRARY_PHOTO_SOURCE_H

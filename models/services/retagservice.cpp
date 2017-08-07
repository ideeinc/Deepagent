#include "retagservice.h"
#include "tag.h"
#include "tagrepository.h"
#include "containers/retagsearchcontainer.h"
#include "containers/retagshowcontainer.h"
#include "managedfile.h"
#include "jpeginfo.h"


RetagService::RetagService()
{}


static QStringList intersect(const QStringList &images1, const QStringList &images2)
{
    QStringList a, b;
    if (images1.count() < images2.count()) {
        a = images1;
        b = images2;
    } else {
        a = images2;
        b = images1;
    }

    for (int i = a.count() - 1; i >= 0; i--) {
        if (! b.contains(a[i])) {
            a.removeAt(i);
        }
    }
    return a;
}


RetagSearchContainer RetagService::search(const QString &tagName1, const QString &tagName2, const QString &tagName3, TSession& session)
{
    RetagSearchContainer container;
    TagRepository repo;

    Tag tag1 = repo.findTag(tagName1);
    Tag tag2 = repo.findTag(tagName2);
    Tag tag3 = repo.findTag(tagName3);

    if ((!tagName1.isEmpty() && !tag1.exists()) || (!tagName2.isEmpty() && !tag2.exists()) || (!tagName3.isEmpty() && !tag3.exists())) {
        tWarn() << "invalid tag specified.";
        return container;
    }

    auto images = tag1.imageNames();
    if (tag2.exists()) {
        images = intersect(images, tag2.imageNames());
    }
    if (tag3.exists()) {
        images = intersect(images, tag3.imageNames());
    }

    tDebug() << "image count: " << images.count();
    container.images = images;
    container.groups = repo.allGroups();
    // フォルダタググループを除外
    for (int i = container.groups.count() - 1; i >= 0; i--) {
        auto name = container.groups[i].name();
        if (name.startsWith(QString::fromUtf8(u8"フォルダ"))
             || name.startsWith(QString::fromUtf8(u8"システム"))) {
            container.groups.removeAt(i);
        }
    }
    return container;
}


RetagShowContainer RetagService::show(const QString &image, TSession& session)
{
    RetagShowContainer container;
    TagRepository repo;
    QList<TagGroup> excludes = repo.allGroups();

    // フォルダタググループを設定
    for (int i = excludes.count() - 1; i >= 0; i--) {
        if (! excludes[i].name().startsWith(QString::fromUtf8(u8"フォルダ"))) {
            excludes.removeAt(i);
        }
    }

    auto meta = ManagedFile::fromFileName(image);
    container.imagePath = ManagedFile::fileNameToPublicPath(image);
    container.originalName = meta.name();
    container.imageSize = JpegInfo(meta.path()).size();
    container.tags = repo.getTags(image, excludes);
    return container;
}

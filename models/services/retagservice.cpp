#include "retagservice.h"
#include "tag.h"
#include "tagrepository.h"
#include "tsession.h"
#include "containers/retagsearchcontainer.h"
#include "containers/retagshowcontainer.h"
#include "managedfile.h"
#include "jpeginfo.h"
#include <QSet>

#define RetagSequentialImages "RetagSequentialImages"
#define RetagSearchPath       "RetagSearchPath"


RetagService::RetagService()
{}


static QStringList intersect(const QStringList &images1, const QStringList &images2)
{
    QStringList imgs;
    QSet<QString> set;

    if (images1.count() < images2.count()) {
        imgs = images1;
        set = QSet<QString>::fromList(images2);
    } else {
        imgs = images2;
        set = QSet<QString>::fromList(images1);
    }

    for (int i = imgs.count() - 1; i >= 0; i--) {
        if (! set.contains(imgs[i])) {
            imgs.removeAt(i);
        }
    }
    imgs.sort();
    return imgs;
}


RetagSearchContainer RetagService::search(const QString &tagName0, const QString &tagName1, const QString &tagName2,
                                          THttpRequest &request, TSession& session)
{
    RetagSearchContainer container;
    TagRepository repo;

    Tag tag0 = repo.findTag(tagName0);
    Tag tag1 = repo.findTag(tagName1);
    Tag tag2 = repo.findTag(tagName2);

    if ((!tagName0.isEmpty() && !tag0.exists()) || (!tagName1.isEmpty() && !tag1.exists()) || (!tagName2.isEmpty() && !tag2.exists())) {
        tWarn() << "invalid tag specified.";
        return container;
    }

    container.selectedTag[0] = tag0;
    container.selectedTag[1] = tag1;
    container.selectedTag[2] = tag2;

    auto images = tag0.imageNames();
    if (tag1.exists() && tag1 != tag0) {
        images = intersect(images, tag1.imageNames());
    }
    if (tag2.exists() && tag2 != tag0 && tag2 != tag1) {
        images = intersect(images, tag2.imageNames());
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
    container.imageCount = container.images.count();

    container.pager.setItemTotalCount(container.imageCount);
    container.pager.setItemCountPerPage(98);
    container.pager.setMidRange(11);
    int current = request.queryItemValue("p", "1").toInt();
    container.pager.setCurrentPage(current);  // 現在のページを設定

    int offset = container.pager.offset();
    container.images = container.images.mid(offset, 98);
    return container;
}


static RetagShowContainer createShowContainer(const QString &image)
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

    auto mngf = ManagedFile::fromFileName(image);
    container.imagePath = ManagedFile::fileNameToPublicPath(image);
    container.originalName = mngf.name();
    container.imageSize = JpegInfo(mngf.path()).size();
    container.tags = mngf.getTags(excludes);
    return container;
}


RetagShowContainer RetagService::startSequential(const QString &recentPath, THttpRequest &request, TSession& session)
{
    auto images = request.formItemList("images[]");
    if (images.isEmpty()) {
        return RetagShowContainer();
    }
    session.insert(RetagSequentialImages, images);
    auto image = images.first();
    auto container = createShowContainer(image);
    container.queueCount = images.count();
    container.backPath = recentPath;
    session.insert(RetagSearchPath, recentPath);
    return container;
}


RetagShowContainer RetagService::sequential(const QString &index, THttpRequest &request, TSession &session)
{
    auto images = session.value(RetagSequentialImages).toStringList();
    if (images.isEmpty()) {
        return RetagShowContainer();
    }

    int idx = index.toInt();
    auto image = images.value(idx);
    auto container = createShowContainer(image);
    container.index = idx;
    container.queueCount = images.count();
    container.backPath = session.value(RetagSearchPath).toString();
    return container;
}


RetagShowContainer RetagService::show(const QString &image, TSession &)
{
    auto container = createShowContainer(image);
    container.index = 0;
    container.queueCount = 1;
    return container;
}

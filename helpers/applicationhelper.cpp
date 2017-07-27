#include "applicationhelper.h"

#define SETTING(key) (Tf::conf("settings").value(#key).toString())


class PathMap : public QList<QPair<QString, QString>>
{
public:
    PathMap()
    {
        append(qMakePair(SETTING(OriginalImagesDir), SETTING(OriginalImagesPublicPath)));
        append(qMakePair(SETTING(TagsDir), SETTING(TagsPublicPath)));
    }
};
Q_GLOBAL_STATIC(PathMap, pathMap)


QString ApplicationHelper::convertToPublic(const QString &realPath)
{
    QString path = realPath;

    for (auto &p : *pathMap()) {
        if (path.startsWith(p.first)) {
            path.remove(0, p.first.length());
            path.prepend(p.second);
            break;
        }
    }
    return path;
}

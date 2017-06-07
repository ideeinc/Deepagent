#include "services/datasetmakerservice.h"
#include "services/taggroup.h"
#include <TWebApplication>
#include <THttpRequest>
#include <functional>


namespace {
    void combination_internal(const long level, const QList<TagGroup>& sourceGroups, const QList<Tag>& tags, const std::function<void(const QList<Tag>)>& callback)
    {
        const TagGroup currentGroup = sourceGroups[ level ];
        for (const Tag& target : currentGroup.tags()) {
            QList<Tag> currentTags(tags);
            currentTags << target;
            const long nextLevel = (level + 1);
            if (nextLevel < sourceGroups.count()) {
                combination_internal(nextLevel, sourceGroups, currentTags, callback);
            }
            else {
                callback(currentTags);
            }
        }
    }
    void combination(const QList<TagGroup>& sourceGroups, const std::function<void(const QList<Tag>)>& callback)
    {
        QList<Tag> empty;
        combination_internal(0, sourceGroups, empty, callback);
    }
}


DatasetMakerIndexContainer DatasetMakerService::index()
{
    DatasetMakerIndexContainer container;

    const QString dirPath(QDir(Tf::app()->configPath()).filePath("datasetmaker"));
    QFile file( QDir(dirPath).absoluteFilePath("default.json") );
    if (file.open(QFile::ReadOnly)) {
        QJsonObject setting = QJsonDocument::fromJson(file.readAll()).object();
        for (const auto& v : setting.value("excludes").toArray()) {
            QJsonObject entry = v.toObject();
            for (const auto& k : entry.keys()) {
                container.excludes << TagGroup(k).tag(entry.value(k).toString());
            }
        }
        file.close();
    }

    return container;
}

DatasetMakerPreviewContainer DatasetMakerService::preview(THttpRequest& request)
{
    DatasetMakerPreviewContainer container;

    QMap<QString, QSet<QString>> cache;
    QList<TagGroup> required;
    for (const auto& group : request.formItemList("groups")) {
        const TagGroup g(group);
        required << g;
        for (const Tag& t : g.tags()) {
            cache[ g.name() + "_" + t.name() ] = t.imageNames().toSet();
        }
    }

    QSet<QString> excludes;
    for (const auto& info : request.formItemList("excludes")) {
        const QVariantMap m = QJsonDocument::fromJson(info.toUtf8()).object().toVariantMap();
        for (const auto& k : m.keys()) {
            excludes += TagGroup(k).tag(m.value(k).toString()).imageNames().toSet();
        }
    }

    combination(required, [&](const QList<Tag> list){
        QString name, displayName;
        QSet<QString> images;
        for (long i = 0; i < list.count(); ++i) {
            const Tag& t = list[i];
            const QString cacheIdentifier = t.groupName() + "_" + t.name();
            if (i == 0)
                images = cache[ cacheIdentifier ];
            else
                images &= cache[ cacheIdentifier ];

            if (! name.isEmpty())
                name += "_";
            name += t.name();
            if (! displayName.isEmpty())
                displayName += " x ";
            displayName += t.displayName();
        }
        if (! excludes.isEmpty()) {
            images -= excludes;
        }
        if (! images.isEmpty()) {
            container.list.append(QVariantMap{{ "images", QVariant(images.toList()) }, { "name", name }, { "displayName", displayName }});
        }
    });

    return container;
}

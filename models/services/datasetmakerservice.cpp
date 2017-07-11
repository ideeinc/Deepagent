#include "services/datasetmakerservice.h"
#include "services/taggroup.h"
#include "logics/dirlayoutdescriptor.h"
#include "logics/archiver.h"
#include <TWebApplication>
#include <THttpRequest>
#include <TSession>
#include <QtConcurrent>
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
        if (sourceGroups.count() > 0) {
            QList<Tag> empty;
            combination_internal(0, sourceGroups, empty, callback);
        }
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

    const QDir archiveDir(Tf::conf("settings").value("DatasetArchivesDir").toString());
    for (const QString& archive : archiveDir.entryList({"*.tar.gz"})) {
        container.archives << archive;
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
            cache[ g.name() + "_" + t.name() ] = t.imagePaths().toSet();
        }
    }

    QSet<QString> excludes;
    for (const auto& info : request.formItemList("excludes")) {
        const QVariantMap m = QJsonDocument::fromJson(info.toUtf8()).object().toVariantMap();
        for (const auto& k : m.keys()) {
            excludes += TagGroup(k).tag(m.value(k).toString()).imagePaths().toSet();
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

DatasetMakerResultContainer DatasetMakerService::make(THttpRequest& request, const QVariantList& originalList, const QString& archiveName)
{
    DatasetMakerResultContainer container;
    QMap<QString, QStringList> sources;

    const QVariantMap labelNames = request.formItems("labelNames");
    const QVariantMap includes = request.formItems("includes");
    for (const QVariant& k : includes.values()) {
        const QString identifier = k.toString();
        const QString labelName = labelNames[ identifier ].toString();
        for (const QVariant& l : originalList) {
            const QVariantMap m = l.toMap();
            if (m["name"] == identifier) {
                sources[ labelName ] << m["images"].toStringList();
                break;
            }
        }
    }

    const QTemporaryDir workDir;
    const QString datasetName = (archiveName.isEmpty() ? request.formItemValue("datasetName") : archiveName);
    const QString workPath = QDir(workDir.path()).filePath(datasetName); QDir().mkpath(workPath);

    const int augmentationRate = request.formItemValue("augmentationRate").toInt();
    const float validationRate = request.formItemValue("validationRate").toFloat() / 100.0;
    const QSize size = request.hasFormItem("imageSize") ? QSize(request.formItems("imageSize")["w"].toInt(), request.formItems("imageSize")["h"].toInt()) : QSize(448, 448);

    const QString layoutType = request.formItemValue("layoutType");
    std::unique_ptr<DirLayoutDescriptor> descriptor = DirLayoutFactory::create(layoutType);
    if (nullptr != descriptor) {
        descriptor->setAugmentationRate(augmentationRate);
        descriptor->setValidationRate(validationRate);
        descriptor->setImageSize(size);

        descriptor->layout(sources, workPath);
        descriptor.reset();
    }

    const QString archiveType = request.formItemValue("archiveType");
    std::unique_ptr<Archiver> archiver = ArchiverFactory::create(archiveType);
    if (nullptr != archiver) {
        archiver->setName(datasetName);
        archiver->setImageSize(size);
        archiver->setLayoutType(layoutType);
        archiver->setLabelNames(sources.keys());

        const QString archiveDir(Tf::conf("settings").value("DatasetArchivesDir").toString());
        container.path = archiver->archive(workPath, archiveDir);
        archiver.reset();
    }

    return container;
}

QVariantMap DatasetMakerService::check(TSession& session, const QString& action)
{
    QVariantMap response;

    const QString path = QDir(Tf::app()->tmpPath()).filePath(session.value(action + "Result").toString());
    response["status"] = QFileInfo(path).exists();
    response["file"] = path;

    return response;
}

void DatasetMakerService::previewAsync(THttpRequest& request, TSession& session)
{
    const QString oldPreviewName = session.value("previewResult").toString();
    if (QFileInfo( QDir(Tf::app()->tmpPath()).filePath(oldPreviewName) ).exists()) {
        QFile::remove( QDir(Tf::app()->tmpPath()).filePath(oldPreviewName) );
    }

    session.remove("previewResult");
    const QString previewName("datasetpreview_" + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));

    QtConcurrent::run([=]{
        THttpRequest r(request);

        const DatasetMakerPreviewContainer container = preview(r);
        const QJsonDocument json = QJsonDocument::fromVariant(container.list);

        QFile file( QDir(Tf::app()->tmpPath()).filePath(previewName) );
        if (file.open(QFile::WriteOnly)) {
            file.write(json.toJson());
            file.close();
        }
    });

    session.insert("previewResult", previewName);
}

DatasetMakerPreviewContainer DatasetMakerService::previewResult(TSession& session)
{
    DatasetMakerPreviewContainer container;

    const QString path = QDir(Tf::app()->tmpPath()).filePath(session.value("previewResult").toString());
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        container.list = QJsonDocument::fromJson(file.readAll()).array().toVariantList();
        file.close();

        // session.remove("previewResult");
        // file.remove();
    }

    return container;
}

void DatasetMakerService::makeAsync(THttpRequest& request, TSession& session)
{
    session.remove("makeResult");
    const QString archiveName = (request.hasFormItem("datasetName") ?  request.formItemValue("datasetName") : "dataset_" + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));

    const DatasetMakerPreviewContainer result = previewResult(session);
    QtConcurrent::run([=]{
        THttpRequest r(request);
        const DatasetMakerResultContainer container = make(r, result.list, archiveName);

        QFile file(QDir(Tf::app()->tmpPath()).filePath(archiveName));
        if (file.open(QFile::WriteOnly)) {
            file.write(container.path.toUtf8());
            file.close();
        }
    });

    session.insert("makeResult", archiveName);
}

DatasetMakerResultContainer DatasetMakerService::makeResult(TSession& session)
{
    DatasetMakerResultContainer container;

    const QString path = QDir(Tf::app()->tmpPath()).filePath(session.value("makeResult").toString());
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        container.path = file.readAll();
        file.close();

        file.remove();
        session.remove("makeResult");
    }

    return container;
}

QString DatasetMakerService::archivePath(const QString& archiveName)
{
    QString path;

    const QDir archiveDir(Tf::conf("settings").value("DatasetArchivesDir").toString());
    if (archiveDir.exists(archiveName)) {
        path = archiveDir.filePath(archiveName);
    }

    return path;
}

bool DatasetMakerService::removeArchive(const QString& archiveName)
{
    bool success = false;

    const QDir archiveDir(Tf::conf("settings").value("DatasetArchivesDir").toString());
    if (archiveDir.exists(archiveName)) {
        success = QDir(archiveDir).remove(archiveName);
    }

    return success;
}

#include "boundingboxservice.h"
#include "image.h"
#include "logics/tagrepository.h"
#include "logics/managedfilecontext.h"
#include <QtCore>
#include <TWebApplication>
#include <TSession>
#include <THttpRequest>
#include <functional>

#define IMAGE_LIST_KEY  "BoundingBox_ImageListKey"
#define SETTING  Tf::conf("settings").value

const QString EngineerUserType = "e";
const QString DoctorUserType = "d";
const QString SuperDoctorUserType = "s";
const QStringList AvailableUserTypes = { EngineerUserType, DoctorUserType, SuperDoctorUserType };


int BoundingBoxService::crop(THttpRequest &, TSession &, int overwrite)
{
    const int cropSize = SETTING("CropSize", 448).toInt();
    const auto uploadImageDir = SETTING("ObjectDetectionImageDir").toString();
    const auto resizedImageDir = SETTING("ObjectDetectionResizedImageDir").toString();
    const QStringList filters = {"*.jpg", "*.jpeg"};

    int resizedCnt = 0;
    QDir imageDir(uploadImageDir);

    if (!imageDir.exists()) {
        return -1;
    }

    auto dirList = imageDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot, QDir::Name);

    for (const auto &dirName : dirList) {
        const QDir dstDir(resizedImageDir + QDir::separator() + dirName);
        if (!dstDir.exists()) {
            dstDir.mkpath(".");
        }

        const QDir subdir(uploadImageDir + QDir::separator() + dirName);
        auto jpgList = subdir.entryInfoList(filters, QDir::Files|QDir::Readable);
        for (const auto &fi : jpgList) {
            QString dstpath = dstDir.absoluteFilePath(fi.fileName());
            Image origimg(fi.absoluteFilePath());
            auto rect = origimg.getValidRect();

            QFileInfo dstFile(dstpath);
            if (overwrite && dstFile.exists()) {
                QFile(dstpath).remove();
            }

            dstFile.refresh();
            if (!dstFile.exists()) {
                Image cropimg = origimg.trimmed();

                if (rect.width() > cropSize && rect.height() > cropSize) {  // まだ大きい
                    if (rect.width() > cropSize * 1.1 && rect.height() > cropSize * 1.1) {
                        // 非常に大きい
                        cropimg.resize(cropSize, cropSize);
                    } else {
                        int x = (cropimg.width() - cropSize)/2;
                        int y = (cropimg.height() - cropSize)/2;
                        cropimg.crop(x, y, cropSize, cropSize);
                    }
                } else if (origimg.width() < cropSize || origimg.height() < cropSize) {  // 元々小さい
                    cropimg = origimg.resized(cropSize, cropSize);
                } else {
                    int x = qMax(rect.x()-(cropSize - rect.width())/2, 0);
                    int y = qMax(rect.y()-(cropSize - rect.height())/2, 0);

                    if (x + cropSize > origimg.width()) {
                        x = origimg.width() - cropSize;
                    }
                    if (y + cropSize > origimg.height()) {
                        y = origimg.height() - cropSize;
                    }
                    cropimg = origimg.cropped(x, y, cropSize, cropSize);
                }

                cropimg.save(dstpath);
                resizedCnt++;
                tDebug() << "resized: " << dstpath;
            }
        }
    }
    return resizedCnt;
}


BoundingBoxSelectTypeContainer BoundingBoxService::selectType(THttpRequest &request, TSession &session, const QString &user)
{
    BoundingBoxSelectTypeContainer container;
    const BoundingBoxSettingContainer prefs = setting(request, session);

    if (!AvailableUserTypes.contains(user)) {
        return container;
    }

    container.user = user;
    container.searchText = session.value("search").toString();

    const TagRepository repository;
    QList<QSet<QString>> excludes;
    for (const auto& groups : prefs.excludeTags) {
        const QVariantMap m = groups.toMap();
        for (const auto& i : m.keys()) {
            excludes << repository.findGroup(i).findTag(m.value(i).toString()).imageNames().toSet();
        }
    }

    const TagGroup colGroup = repository.findGroup(prefs.columnGroup);
    QList<Tag> cols = colGroup.tags();
    qSort(cols.begin(), cols.end(), [=](const Tag& t1, const Tag& t2) -> bool {
        return prefs.columnOrders.indexOf(t1.name()) < prefs.columnOrders.indexOf(t2.name());
    });

    if (user == DoctorUserType) {
        cols.erase(std::remove_if(cols.begin(), cols.end(), [=](const Tag& s) -> bool {
            return prefs.authorizationRequiredColumns.contains(s.name());
        }), cols.end());
    }

    std::transform(cols.begin(), cols.end(), std::back_inserter(container.cols), [](const Tag& t){
        return t.displayName();
    });

    QList<QSet<QString>> columns;
    for (const Tag& colTag : cols) {
        columns << colTag.imageNames().toSet();
    }

    const TagGroup rowGroup = repository.findGroup(prefs.rowGroup);
    if (rowGroup.exists() && colGroup.exists()) {
        container.listGroupName = rowGroup.name();
        for (const auto& rowTag : rowGroup.tags()) {
            QSet<QString> folder = rowTag.imageNames().toSet();
            for (const auto& exclude : excludes) {
                folder -= exclude;
            }
            if (container.searchText.isEmpty() || (folder.count() > 0)) {
                QSet<QString> notags(folder); // copy all.

                QVariantMap content;
                content["name"] = rowTag.name();
                content["displayName"] = rowTag.displayName();

                QVariantList list;
                for (long i = 0; i < columns.count(); ++i) {
                    QSet<QString> imageNames = columns[i];

                    if (! container.searchText.isEmpty()) {
                        imageNames = ManagedFileContext::filterInList(container.searchText, imageNames.toList()).toSet();
                        notags = ManagedFileContext::filterInList(container.searchText, notags.toList()).toSet();
                    }

                    list.append(QVariantMap{{ "name", cols[i].name() }, { "count", imageNames.intersect(folder).count() }});
                    notags -= imageNames;
                }

                content["list"] = list;             // each column tagged images.
                content["notag"] = notags.count();  // no column tagged images.
                content["all"] = folder.count();    // all row images.

                container.rows.append(content);
            }
        }
    }
    return container;
}


QStringList BoundingBoxService::readImage(THttpRequest &request, TSession &session, const QString &user, const QString &dir)
{
    QStringList imageList;

    if (!AvailableUserTypes.contains(user)) {
        return imageList;
    }

    const TagRepository repository;
    const BoundingBoxSettingContainer prefs = setting(request, session);
    const Tag folder = repository.findGroup(prefs.rowGroup).findTag(dir);
    if (! folder.exists()) {
        return imageList;
    }

    QSet<QString> imageNames = folder.imageNames().toSet();

    for (const auto& groups : prefs.excludeTags) {
        const QVariantMap m = groups.toMap();
        for (const auto& i : m.keys()) {
            imageNames -= repository.findGroup(i).findTag(m.value(i).toString()).imageNames().toSet();
        }
    }

    const QString filter = request.parameter(("filter"));
    const Tag hit = repository.findGroup(prefs.columnGroup).findTag(filter);
    // if column tag is specified, show only intersected images.
    if (hit.exists()) {
        imageNames &= hit.imageNames().toSet();
    }
    // column tag is not found but not empty, show only subtracted images (no column tag).
    else if (! filter.isEmpty()) {
        for (const Tag& t : repository.findGroup(prefs.columnGroup).tags()) {
            imageNames -= t.imageNames().toSet();
        }
    }

    QString searchText;
    if (session.contains("search")) {
        searchText = session.value("search").toString();
    }
    if (request.hasQueryItem("search")) {
        searchText = request.parameter("search");
    }
    if (! searchText.isEmpty()) {
        imageNames = ManagedFileContext::filterInList(searchText, imageNames.toList()).toSet();
    }

    if (imageNames.count() > 0) {
        for (const auto &name : imageNames) {
            imageList << QDir(folder.path()).filePath(name);
        }

        QCollator collator;
        collator.setNumericMode(true);
        qSort(imageList.begin(), imageList.end(), [&collator](const QString& p1, const QString& p2){
            return (collator.compare(ManagedFile::fromLink(p1).name(), ManagedFile::fromLink(p2).name()) < 0);
        });
        session.insert(IMAGE_LIST_KEY, imageList);
    }
    return imageList;
}


BoundingBoxDrawContainer BoundingBoxService::draw(THttpRequest &request, TSession &session, const QString &user, const QString &dir, const QString &index)
{
    BoundingBoxDrawContainer container;

    if (!AvailableUserTypes.contains(user)) {
        return container;
    }

    const TagRepository repository;
    const BoundingBoxSettingContainer prefs = setting(request, session);
    const Tag folder = repository.findGroup(prefs.rowGroup).findTag(dir);

    container.user = user;
    container.group = folder.groupName();
    container.label = folder.name();
    container.displayName = folder.displayName();

    auto imageList = session.value(IMAGE_LIST_KEY).toStringList();
    int idx = index.toInt();
    if (imageList.isEmpty() || idx < 0 || idx >= imageList.count()) {
        return container;
    }

    container.index = idx;
    container.jpegCount = imageList.count();
    container.jpegFile.setFile(imageList[idx]);
    container.originalName = ManagedFile::fromLink(imageList[idx]).name();
    container.prevIndex = idx - 1;
    container.nextIndex = idx + 1;

    if (!container.jpegFile.exists()) {
        container.jpegFile = QFileInfo();
        tError() << "file not found: " << imageList[idx];
    }
    else {
        const Image originalImage(container.jpegFile.absoluteFilePath());
        container.originalSize = QSize(originalImage.width(), originalImage.height());
    }
    return container;
}

BoundingBoxSettingContainer BoundingBoxService::setting(THttpRequest& request, TSession&)
{
    BoundingBoxSettingContainer container;

    QVariantMap setting;

    // load settings
    const QDir systemDirPath(QDir(Tf::app()->configPath()).filePath("object-detection"));
    if (systemDirPath.exists("config.json")) {
        QFile loadFile(systemDirPath.absoluteFilePath("config.json"));
        if (loadFile.open(QFile::ReadOnly)) {
            setting = QJsonDocument::fromJson(loadFile.readAll()).object().toVariantMap();
            loadFile.close();
        }
    }

    // load overloaded settings
    const QDir runtimeDirPath(SETTING("ObjectDetectionConfigDir").toString());
    if (runtimeDirPath.exists("config.json")) {
        QFile loadFile(runtimeDirPath.absoluteFilePath("config.json"));
        if (loadFile.open(QFile::ReadOnly)) {
            const QVariantMap p = QJsonDocument::fromJson(loadFile.readAll()).object().toVariantMap();
            for (auto i = p.begin(); i != p.end(); ++i) {
                setting.insert(i.key(), i.value());
            }
            loadFile.close();
        }
    }

    if (request.method() == Tf::Post) {
        // override with request
        if (request.hasFormItem("rowGroup")) {
            const QString rowGroup = request.formItemValue("rowGroup");
            if ((! rowGroup.isEmpty()) && (rowGroup != setting["RowTagGroup"])) setting["RowTagGroup"] = rowGroup;
        }
        if (request.hasFormItem("columnGroup")) {
            const QString columnGroup = request.formItemValue("columnGroup");
            if ((! columnGroup.isEmpty()) && (columnGroup != setting["ColumnTagGroup"])) setting["ColumnTagGroup"] = columnGroup;
        }
        if (request.hasFormItem("excludeTags[]")) {
            QVariantList list;
            const QStringList excludes = request.formItemList("excludeTags");
            for (const auto& definition : excludes) {
                if (! definition.isEmpty()) {
                    list << QJsonDocument::fromJson(definition.toUtf8()).object().toVariantMap();
                }
            }
            setting["ExcludedTags"] = list;
        }
        if (request.hasFormItem("authorizationRequiredColumns[]")) {
            QStringList authorizationRequiredColumns = request.formItemList("authorizationRequiredColumns");
            authorizationRequiredColumns.erase(std::remove_if(authorizationRequiredColumns.begin(), authorizationRequiredColumns.end(), [](const QString& s){
                return s.isEmpty();
            }), authorizationRequiredColumns.end());
            setting["AuthRequiredCols"] = authorizationRequiredColumns;
        }
        if (request.hasFormItem("columnOrders[]")) {
            QStringList columnOrders = request.formItemList("columnOrders");
            columnOrders.erase(std::remove_if(columnOrders.begin(), columnOrders.end(), [](const QString& s){
                return s.isEmpty();
            }), columnOrders.end());
            setting["ColumnOrders"] = columnOrders;
        }
        if (request.hasFormItem("savedActionTags[e]") || request.hasFormItem("savedActionTags[d]")) {
            const QVariantMap savedActionTags = request.formItems("savedActionTags");
            setting["SavedActionTags"] = savedActionTags;
        }
        if (request.hasFormItem("labelGroups[]")) {
            QStringList labelGroups = request.formItemList("labelGroups");
            labelGroups.erase(std::remove_if(labelGroups.begin(), labelGroups.end(), [](const QString& s){
                return s.isEmpty();
            }), labelGroups.end());
            setting["DatasetLabelGroups"] = labelGroups;
        }

        // save settings
        runtimeDirPath.mkpath(".");
        QFile saveFile(runtimeDirPath.absoluteFilePath("config.json"));
        if (saveFile.open(QFile::WriteOnly)) {
            saveFile.write(QJsonDocument(QJsonObject::fromVariantMap(setting)).toJson());
            saveFile.close();
        }
    }

    container.rowGroup = setting.value("RowTagGroup").toString();
    container.columnGroup = setting.value("ColumnTagGroup").toString();
    container.excludeTags = setting.value("ExcludedTags").toList();
    container.authorizationRequiredColumns = setting.value("AuthRequiredCols").toStringList();
    container.columnOrders = setting.value("ColumnOrders").toStringList();
    container.savedActionTags = setting.value("SavedActionTags").toMap();
    container.labelGroups = setting.value("DatasetLabelGroups").toStringList();

    return container;
}

QVariantList BoundingBoxService::readAreas(THttpRequest &, TSession &session, const QString &, const QString &, const long &index)
{
    QList<QVariant> boxes;

    const auto objectDetectionLabelDir = SETTING("ObjectDetectionLabelDir").toString();
    const QDir labelDir(objectDetectionLabelDir);

    const QStringList images = session.value(IMAGE_LIST_KEY).toStringList();
    if ((! images.isEmpty()) && (index >= 0) && (images.count() > index)) {
        QFile label(labelDir.absoluteFilePath(QFileInfo(images[index]).completeBaseName() + ".txt"));
        if (label.exists() && label.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&label);
            while (! stream.atEnd()) {
                // 0:type(label), 1:truncated, 2:occluded, 3:alpha, 4:left, 5:top, 6:right, 7:bottom, 8-10:dimensions, 11-13:rotation_y
                const QStringList line = stream.readLine().split(' ');

                QVariantMap box;
                box["type"] = QVariant(line[0]);
                box["truncated"] = QVariant(line[1].toFloat());
                box["occluded"] = QVariant(line[2].toInt());
                box["alpha"] = QVariant(line[3].toFloat());
                box["bbox_x"] = QVariant(line[4].toFloat());
                box["bbox_y"] = QVariant(line[5].toFloat());
                box["bbox_w"] = QVariant(line[6].toFloat() - line[4].toFloat());
                box["bbox_h"] = QVariant(line[7].toFloat() - line[5].toFloat());
                box["dimension_h"] = QVariant(line[8].toFloat());
                box["dimension_w"] = QVariant(line[9].toFloat());
                box["dimension_l"] = QVariant(line[10].toFloat());
                box["location_x"] = QVariant(line[11].toFloat());
                box["location_y"] = QVariant(line[12].toFloat());
                box["location_z"] = QVariant(line[13].toFloat());
                box["rotation_y"] = QVariant(line[14].toFloat());
                if (line.count() > 15) {
                    box["score"] = QVariant(line[15].toFloat());
                }
                boxes.append(box);
            }
            label.close();
        }
    }

    return boxes;
}

void BoundingBoxService::saveAreas(THttpRequest &request, TSession &session, const QString &user, const QString &dir, const long &index, const QList<QVariantMap> &areas)
{
    const auto objectDetectionLabelDir = SETTING("ObjectDetectionLabelDir").toString();
    const QDir labelDir(objectDetectionLabelDir);
    if (! labelDir.exists()) {
        QDir().mkpath(objectDetectionLabelDir);
    }

    const QStringList images = session.value(IMAGE_LIST_KEY).toStringList();
    if ((! images.isEmpty()) && (index >= 0) && (images.count() > index)) {
        const QString labelFileName = QFileInfo(images[index]).completeBaseName() + ".txt";

        // save label with kitti-format.
        QFile label(labelDir.absoluteFilePath(labelFileName));
        if (label.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            QTextStream stream(&label);
            for (const QVariantMap& v : areas) {
                stream << dir << " "
                    << QString::number(v["truncated"].toFloat(), 'f', 2) << " "
                    << QString::number(v["occluded"].toInt()) << " "
                    << QString::number(v["alpha"].toFloat(), 'f', 2) << " "
                    << QString::number(v["bbox_x"].toFloat(), 'f', 2) << " "
                    << QString::number(v["bbox_y"].toFloat(), 'f', 2) << " "
                    << QString::number(v["bbox_w"].toFloat() + v["bbox_x"].toFloat(), 'f', 2) << " "
                    << QString::number(v["bbox_h"].toFloat() + v["bbox_y"].toFloat(), 'f', 2) << " "
                    << QString::number(v["dimension_h"].toFloat() + v["dimension_h"].toFloat(), 'f', 2) << " "
                    << QString::number(v["dimension_w"].toFloat() + v["dimension_w"].toFloat(), 'f', 2) << " "
                    << QString::number(v["dimension_l"].toFloat() + v["dimension_l"].toFloat(), 'f', 2) << " "
                    << QString::number(v["location_x"].toFloat() + v["location_x"].toFloat(), 'f', 2) << " "
                    << QString::number(v["location_y"].toFloat() + v["location_y"].toFloat(), 'f', 2) << " "
                    << QString::number(v["location_z"].toFloat() + v["location_z"].toFloat(), 'f', 2) << " "
                    << QString::number(v["rotation_y"].toFloat(), 'f', 2);
                    if (v.contains("score")) {
                       stream << " " << QString::number(v["score"].toFloat(), 'f', 2);
                    }
                    stream << "\n";
            }
            label.close();
        }

        // update tag to new state.
        TagRepository repository;
        const BoundingBoxSettingContainer prefs = setting(request, session);
        if (prefs.savedActionTags.contains(user)) {
            const Tag nextColumn = repository.findGroup(prefs.columnGroup).findTag(prefs.savedActionTags.value(user).toString());
            if (nextColumn.exists()) {
                repository.updateImages({ images[index] }, {{ nextColumn.groupName(), nextColumn.name() }});
            }
        }
    }
}

long BoundingBoxService::moveTrash(THttpRequest& request, TSession& session, const QString&, const QString&, const long& index)
{
    long position = index;

    QStringList images = session.value(IMAGE_LIST_KEY).toStringList();
    if ((! images.isEmpty()) && (index >= 0) && (images.count() > index)) {
        const QFileInfo imageFile = QFileInfo(images[index]);

        const TagRepository repository;
        const BoundingBoxSettingContainer prefs = setting(request, session);
        for (const auto& groups : prefs.excludeTags) {
            const QVariantMap m = groups.toMap();
            for (const auto& i : m.keys()) {
                const Tag tag = repository.findGroup(i).findTag(m.value(i).toString());
                TagRepository().updateImages({ imageFile.absoluteFilePath() }, {{ tag.groupName(), tag.name() }});
            }
        }

        // determine next shown position
        const long rest = images.count() - 1;
        if (index < rest) {
            position = index;
        }
        else if (index == rest) { // && (0 < index)
            position = index - 1;
        }
        else {
            position = -1;
        }

        images.removeAt(index);
        session[IMAGE_LIST_KEY] = images;
    }

    return position;
}

BoundingBoxMakeDatasetContainer BoundingBoxService::makeDatasetContainer(THttpRequest&, TSession&)
{
    BoundingBoxMakeDatasetContainer container;

    const auto objectDetectionLabelDir = SETTING("ObjectDetectionLabelDir").toString();
    const auto objectDetectionCheckedLabelDir = SETTING("ObjectDetectionCheckedLabelDir").toString();
    const auto objectDetectionResizedImageDir = SETTING("ObjectDetectionResizedImageDir").toString();
    const auto objectDetectionTrashDir = SETTING("ObjectDetectionTrashDir").toString();
    const auto objectDetectionArchiveDir = SETTING("ObjectDetectionArchiveDir").toString();

    // load default available labels.
    QFile labelDefs(QDir(Tf::app()->configPath() + QDir::separator() + "object-detection").absoluteFilePath("labels.json"));
    if (labelDefs.open(QFile::ReadOnly)) {
        QVariantMap m = QJsonDocument::fromJson(labelDefs.readAll()).object().toVariantMap();
        for (const QString& k : m.keys()) {
            container.availableLabels[ k ] = m.value(k).toString();
        }
    }
    // overload available labels (if 'labels/default.json' exists).
    if (QDir(objectDetectionLabelDir).exists("default.json")) {
        QFile overloadMap(QDir(objectDetectionLabelDir).absoluteFilePath("default.json"));
        if (overloadMap.open(QFile::ReadOnly)) {
            QVariantMap m = QJsonDocument::fromJson(overloadMap.readAll()).object().toVariantMap();
            for (const QString& k : m.keys()) {
                container.availableLabels[ k ] = m.value(k).toString();
            }
            overloadMap.close();
        }
    }

    // load labels last saved.
    if (QDir(objectDetectionArchiveDir).exists("labels.json")) {
        // from 'archive/labels.json' (last dataset created)
        QFile lastMap(QDir(objectDetectionArchiveDir).absoluteFilePath("labels.json"));
        if (lastMap.open(QFile::ReadOnly)) {
            QVariantMap m = QJsonDocument::fromJson(lastMap.readAll()).object().toVariantMap();
            for (const QString& k : m.keys()) {
                container.savedLabels[ k ] = m.value(k).toString();
            }
            lastMap.close();
        }
    }
    else if (QDir(objectDetectionResizedImageDir).exists("labels.json")) {
        // from 'resized/labels.json' (user created)
        QFile alternateMap(QDir(objectDetectionResizedImageDir).absoluteFilePath("labels.json"));
        if (alternateMap.open(QFile::ReadOnly)) {
            QVariantMap m = QJsonDocument::fromJson(alternateMap.readAll()).object().toVariantMap();
            for (const QString& k : m.keys()) {
                container.savedLabels[ k ] = m.value(k).toString();
            }
            alternateMap.close();
        }
    }

    // directory information.
    QDir labelDir(objectDetectionLabelDir);
    if (labelDir.exists()) {
        for (const auto& info : labelDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name)) {
            const QString name = info.fileName();
            if (QDir(objectDetectionResizedImageDir).exists(name)) {
                const long count = QDir(info.absoluteFilePath()).entryList(QDir::Files|QDir::NoDotAndDotDot).count()
                                        - QDir(objectDetectionCheckedLabelDir + QDir::separator() + name).entryList(QDir::Files|QDir::NoDotAndDotDot).count()
                                        - QDir(objectDetectionTrashDir + QDir::separator() + name).entryList(QDir::Files|QDir::NoDotAndDotDot).count();
                if (count > 0) {
                    container.directories.insert(name, count);
                }
            }
        }
    }

    return container;
}

QString BoundingBoxService::makeDataset(THttpRequest& request, TSession& session, const QVariantMap& datasetInfo, const bool isAugmentation)
{
    const auto archiveName = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss") + "-" + QUuid::createUuid().toString().split("-")[0].replace(0, 1, "");

    const auto objectDetectionArchiveDir = SETTING("ObjectDetectionArchiveDir").toString();
    const QDir archiveDir(objectDetectionArchiveDir + QDir::separator() + archiveName);
    if (! archiveDir.exists()) {
        QDir(objectDetectionArchiveDir).mkpath(archiveName);
    }
    archiveDir.mkpath(QString("train") + QDir::separator() + "images");
    archiveDir.mkpath(QString("train") + QDir::separator() + "labels");

    const auto objectDetectionLabelDir = SETTING("ObjectDetectionLabelDir").toString();
    const auto objectDetectionCheckedLabelDir = SETTING("ObjectDetectionCheckedLabelDir").toString();
    const auto objectDetectionResizedImageDir = SETTING("ObjectDetectionResizedImageDir").toString();
    const auto objectDetectionTrashDir = SETTING("ObjectDetectionTrashDir").toString();

    const QDir trainImageDir = QDir(archiveDir.absoluteFilePath("train") + QDir::separator() + "images");
    const QDir trainLabelDir = QDir(archiveDir.absoluteFilePath("train") + QDir::separator() + "labels");

    const QVariantMap labelMap = datasetInfo["labels"].toMap();
    QVariantMap normalizedLabelMap; // for Safari's UTF-8 NFC.
    // make training data.
    for (const QString& dirName : labelMap.keys()) {
        const QString normalizedDirName = dirName.normalized(QString::NormalizationForm_D);
        for (const QFileInfo& label : QDir(objectDetectionLabelDir + QDir::separator() + normalizedDirName).entryInfoList({"*.txt"}, QDir::Files|QDir::Readable)) {
            const QString basename = label.completeBaseName();
            if ((label.size() > 0) &&
                (! QDir(objectDetectionCheckedLabelDir + QDir::separator() + normalizedDirName).exists(basename + ".txt")) &&
                (! QDir(objectDetectionTrashDir + QDir::separator() + normalizedDirName).exists(basename + ".jpg"))) {
                // create label.
                QFile srcLabel(label.absoluteFilePath()), dstLabel(trainLabelDir.absoluteFilePath(basename + ".txt"));
                if (srcLabel.open(QIODevice::ReadOnly) && dstLabel.open(QIODevice::WriteOnly)) {
                    QTextStream reader(&srcLabel), writer(&dstLabel);
                    while (! reader.atEnd()) {
                        const QStringList line = reader.readLine().split(' ');
                        writer << labelMap.value(dirName).toString();
                        for (int i = 1; i < line.length(); ++i) {
                            writer << " " << line[i];
                        }
                        writer << "\n";
                    }
                    srcLabel.close(); dstLabel.close();
                }
                // create image.
                QFile::link(QDir(objectDetectionResizedImageDir + QDir::separator() + normalizedDirName).absoluteFilePath(basename + ".jpg"), trainImageDir.absoluteFilePath(basename + ".jpg"));
            }
        }
        normalizedLabelMap[ normalizedDirName ] = labelMap.value(dirName).toString();
    }
    // save last map
    QFile lastMap(objectDetectionArchiveDir + QDir::separator() + "labels.json");
    if (lastMap.open(QFile::WriteOnly)) {
        lastMap.write(QJsonDocument(QJsonObject::fromVariantMap(normalizedLabelMap)).toJson());
        lastMap.close();
    }

    // move to validation.
    const float validationRate = datasetInfo["validationRate"].toFloat() / 100.0;
    if ((0 < validationRate) && (validationRate < 1)) {
        archiveDir.mkpath(QString("val") + QDir::separator() + "images");
        archiveDir.mkpath(QString("val") + QDir::separator() + "labels");
        const QDir valImageDir = QDir(archiveDir.absoluteFilePath("val") + QDir::separator() + "images");
        const QDir valLabelDir = QDir(archiveDir.absoluteFilePath("val") + QDir::separator() + "labels");
        const QFileInfoList images = trainImageDir.entryInfoList({"*.jpg"});
        const long validationCount = images.count() * validationRate;
        for (int i = 0; i < validationCount; ++i) {
            const QFileInfo& info = images[ qrand() % images.count() ];
            QFile::rename(info.absoluteFilePath(), valImageDir.absoluteFilePath(info.fileName()));
            QFile::rename(trainLabelDir.absoluteFilePath(info.completeBaseName() + ".txt"), valLabelDir.absoluteFilePath(info.completeBaseName() + ".txt"));
        }
    }

    // data augmentation.
    if (isAugmentation) {
        for (const QFileInfo& img : trainImageDir.entryInfoList({"*.jpg"})) {
            for (const QString& angle : QStringList({"90", "180", "270"})) {
                const QString srcImagePath = img.absoluteFilePath();
                const QString dstImagePath = img.absolutePath() + QDir::separator() + img.completeBaseName() + "_" + angle + ".jpg";
                const QString srcLabelPath = trainLabelDir.absoluteFilePath(img.completeBaseName() + ".txt");
                const QString dstLabelPath = trainLabelDir.absoluteFilePath(img.completeBaseName() + "_" + angle + ".txt");
                rotateImageAndLabelFromFile(request, session, srcImagePath, dstImagePath, srcLabelPath, dstLabelPath, angle.toFloat());
            }
        }
    }

    // cleanup old archives
    const int maxNumberOfArchives = 5;
    const QFileInfoList archives = QDir(objectDetectionArchiveDir).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Time | QDir::Reversed);
    if (archives.count() > maxNumberOfArchives) {
        int remain = (archives.count() - maxNumberOfArchives);
        for (const QFileInfo& i : archives) {
            if (QDir(i.absoluteFilePath()).removeRecursively()) {
                remain -= 1;
            }
            if (remain <= 0) break;
        }
    }

    return archiveName;
}

QString BoundingBoxService::makeArchive(THttpRequest&, TSession&, const QString& archiveName)
{
    QString archivePath;

    const auto objectDetectionArchiveDir = SETTING("ObjectDetectionArchiveDir").toString();
    const QDir archiveDir(objectDetectionArchiveDir);

    if (archiveDir.exists(archiveName)) {
        archivePath = archiveDir.absoluteFilePath(archiveName + ".tar.gz");

        QProcess tar;
        tar.start("tar", {"czfh", archivePath, "-C", objectDetectionArchiveDir, archiveName});
        tar.waitForFinished();
    }
    else if (archiveDir.exists(archiveName + ".tar.gz")) {
        archivePath = archiveDir.absoluteFilePath(archiveName + ".tar.gz");
    }

    return archivePath;
}

void BoundingBoxService::rotateImageAndLabelFromFile(THttpRequest& request, TSession& session, const QString &sourceImagePath, const QString &destinationImagePath, const QString &sourceLabelPath, const QString &destinationLabelPath, const float angle)
{
    Image srcImg(sourceImagePath);
    Image dstImg = srcImg.rotated(-angle, 1);
    if (! dstImg.save(destinationImagePath)) {
        return;
    }
    const QPointF center(srcImg.width() / 2.0, srcImg.height() / 2.0);

    QFile srcLabel(sourceLabelPath), dstLabel(destinationLabelPath);
    srcLabel.open(QIODevice::ReadOnly); dstLabel.open(QIODevice::WriteOnly);

    QTextStream reader(&srcLabel), writer(&dstLabel);
    while (! reader.atEnd()) {
        const QString line = reader.readLine();
        writer << rotateLabelFromLineBuffer(request, session, line, angle, center) + "\n";
    }

    srcLabel.close(); dstLabel.close();
}

QString BoundingBoxService::rotateLabelFromLineBuffer(THttpRequest&, TSession&, const QString &lineBuffer, const float angle, const QPointF &center)
{
    const QStringList buffer = lineBuffer.split(' ');

    const QPointF srcTL(buffer[4].toFloat(), buffer[5].toFloat());
    const QPointF srcBR(buffer[6].toFloat(), buffer[7].toFloat());
    const float radian = (angle * M_PI) / 180.0;

    const QPointF dstTL(
        ((srcTL.x() - center.x()) * cos(radian)) - ((srcTL.y() - center.y()) * sin(radian)) + center.x(),
        ((srcTL.x() - center.x()) * sin(radian)) + ((srcTL.y() - center.y()) * cos(radian)) + center.y()
    );
    const QPointF dstBR(
        ((srcBR.x() - center.x()) * cos(radian)) - ((srcBR.y() - center.y()) * sin(radian)) + center.x(),
        ((srcBR.x() - center.x()) * sin(radian)) + ((srcBR.y() - center.y()) * cos(radian)) + center.y()
    );

    const float L = std::min({dstTL.x(), dstBR.x()});
    const float T = std::min({dstTL.y(), dstBR.y()});
    const float R = std::max({dstTL.x(), dstBR.x()});
    const float B = std::max({dstTL.y(), dstBR.y()});

    QStringList output({
        buffer[0], buffer[1], buffer[2], buffer[3],
        QString::number(L, 'f', 2), QString::number(T, 'f', 2), QString::number(R, 'f', 2), QString::number(B, 'f', 2),
        buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14]
    });
    if (buffer.count() > 15) {
        output.append(buffer[15]);
    }
    return output.join(" ");
}

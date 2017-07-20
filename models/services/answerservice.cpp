#include "services/answerservice.h"
#include "logics/tagrepository.h"
#include "image.h"
#include "managedfile.h"
#include <random>
#include <algorithm>

const QString kAnswerSessionKey = "AnswerSession";
const QString kAnswerImagesKey = "AnswerImages";


AnswerService::AnswerService()
{
}

bool
AnswerService::login(const THttpRequest& request, TSession& session)
{
    bool success = false;

    session.remove(kAnswerSessionKey);
    if ((request.method() == Tf::Post) && request.hasFormItem("user")) {
        success = login(session, request.formItemValue("user"), request.formItemValue("tag"));
    }

    return success;
}

bool
AnswerService::login(TSession& session, const QString& user, const QString& tag)
{
    bool success = false;

    if (! user.isEmpty()) {
        session.insert(kAnswerSessionKey, QVariantMap{
            { "user", user },
            { "tag", tag }
        });
        success = true;
    }

    return success;
}

void
AnswerService::logout(TSession& session)
{
    session.remove(kAnswerSessionKey);
}

QString
AnswerService::loginUser(const TSession& session) const
{
    const QVariantMap m = session.value(kAnswerSessionKey).toMap();
    const QString name = m["user"].toString();

    // user name validation
    const QRegularExpression exp("^[a-z][a-z0-9_-]{0,}[a-z0-9]$", QRegularExpression::CaseInsensitiveOption);
    const QRegularExpressionMatch match = exp.match(name);
    if (! match.hasMatch()) {
        return "";
    }

    return name;
}

bool
AnswerService::isLogin(const TSession& session) const
{
    return (! loginUser(session).isEmpty());
}

AnswerConfigContainer
AnswerService::configure(const THttpRequest &request)
{
    AnswerConfigContainer container;

    QVariantMap pref;

    // load default configuration
    const QDir systemConfigDir(QDir(Tf::app()->configPath()).filePath("object-detection"));
    if (systemConfigDir.exists("answer.json")) {
        QFile loadFile(systemConfigDir.absoluteFilePath("answer.json"));
        if (loadFile.open(QFile::ReadOnly)) {
            pref = QJsonDocument::fromJson(loadFile.readAll()).object().toVariantMap();
            loadFile.close();
        }
    }

    // load overloaded configuration
    const QDir runtimeConfigDir(QDir(Tf::conf("settings").value("ObjectDetectionAnswerDir").toString()));
    if (runtimeConfigDir.exists("answer.json")) {
        QFile loadFile(runtimeConfigDir.absoluteFilePath("answer.json"));
        if (loadFile.open(QFile::ReadOnly)) {
            const QVariantMap p = QJsonDocument::fromJson(loadFile.readAll()).object().toVariantMap();
            for (auto i = p.begin(); i != p.end(); ++i) {
                pref.insert(i.key(), i.value());
            }
            loadFile.close();
        }
    }

    // override with request
    if (request.method() == Tf::Post) {
        const QStringList keys{ "examGroup", "answerGroup", "excludeWord", "includeWord", "trustUserName" };
        for (const auto& k : keys) {
            if (request.hasFormItem(k)) {
                pref[k] = request.formItemValue(k);
            }
        }

        // save configuration
        QDir().mkpath(runtimeConfigDir.absolutePath());
        QFile saveFile(runtimeConfigDir.absoluteFilePath("answer.json"));
        if (saveFile.open(QFile::WriteOnly)) {
            saveFile.write(QJsonDocument(QJsonObject::fromVariantMap(pref)).toJson());
            saveFile.close();
        }
    }

    container.examGroup = pref["examGroup"].toString();
    container.answerGroup = pref["answerGroup"].toString();
    container.excludeWord = pref["excludeWord"].toString();
    container.includeWord = pref["includeWord"].toString();
    container.trustUserName = pref["trustUserName"].toString();

    return container;
}

AnswerExamListContainer
AnswerService::selectType(TSession& session)
{
    AnswerExamListContainer container;

    const AnswerConfigContainer config = configure(THttpRequest());
    const TagGroup examGroup = TagRepository().findGroup(config.examGroup);
    if (! examGroup.exists()) {
        return container;
    }

    const QString user = loginUser(session);
    container.user = user;
    container.trusted = (config.trustUserName == user);

    const QDir answerDir( Tf::conf("settings").value("ObjectDetectionAnswerDir").toString() );
    const QDir labelDir(answerDir.filePath(user)); answerDir.mkpath(user);

    for (const Tag & tag : examGroup.tags()) {
        AnswerSelectTypeContainer exam;
        exam.identifier = (container.trusted ? tag.name() : QString(QCryptographicHash::hash(tag.name().toUtf8(), QCryptographicHash::Md5).toHex()).left(7));
        exam.displayName = (container.trusted ? tag.displayName() : exam.identifier);
        for (const QString& image : tag.imagePaths()) {
            if (QFileInfo(image).exists() && (! image.isEmpty()) && (! exam.all.contains(image))) {
                exam.all << image;
                if (labelDir.exists( QFileInfo(image).completeBaseName() + ".txt" )) {
                    exam.checked << image;
                } else {
                    exam.remain << image;
                }
            }
        }
        container.list << exam;
        container.all += exam.all.count();
        container.checked += exam.checked.count();
        container.remain += exam.remain.count();
    }

    if (! container.trusted) {
        qSort(container.list.begin(), container.list.end(), [](const AnswerSelectTypeContainer& k1, const AnswerSelectTypeContainer& k2){
            return (k1.identifier < k2.identifier);
        });
    }

    return container;
}

QStringList
AnswerService::readImages(const QString& identifier, const QString& filter, TSession& session)
{
    QStringList images;
    const AnswerExamListContainer type = selectType(session);
    const auto exam = std::find_if(type.list.begin(), type.list.end(), [&identifier](const AnswerSelectTypeContainer& k) {
        return (k.identifier == identifier);
    });

    if (exam != type.list.end()) {
        if (filter == "checked") {
            images = exam->checked;
        } else if (filter == "remain") {
            images = exam->remain;
        } else {
            images = exam->all;
        }

        QCollator collator;
        collator.setNumericMode(true);
        qSort(images.begin(), images.end(), [&collator](const QString& p1, const QString& p2){
            return (collator.compare(ManagedFile::fromLink(p1).name(), ManagedFile::fromLink(p2).name()) < 0);
        });
    }

    if (images.count() > 0) {
        session.insert(kAnswerImagesKey, images);
    }
    return images;
}

AnswerDisplayImageContainer
AnswerService::displayImage(const TSession& session, const int& index)
{
    AnswerDisplayImageContainer container;

    const QStringList images = session.value(kAnswerImagesKey).toStringList();
    if (images.isEmpty() || (index < 0) || (index >= images.count())) {
        return container;
    }

    const QFileInfo file(images[index]);
    if (file.exists()) {
        const Image jpeg(file.absoluteFilePath());
        container.imageFile = file.absoluteFilePath();
        container.originalFileName = ManagedFile::fromLink(container.imageFile).name();
        container.size = QSize(jpeg.width(), jpeg.height());
        container.amount = images.count();
        container.current = index;
        container.previous = std::max(index - 1, 0);
        container.next = std::min(index + 1, container.amount);

        const AnswerConfigContainer config = configure(THttpRequest());
        container.isTrusted = (config.trustUserName == loginUser(session));
        container.labelTags = config.labelTags();
    }

    return container;
}

QVariantList
AnswerService::readAreas(const TSession& session, const int& index)
{
    QVariantList boxes;

    const QStringList images = session.value(kAnswerImagesKey).toStringList();
    if (images.isEmpty() || (index < 0) || (index >= images.count())) {
        return boxes;
    }

    const QString user = loginUser(session);
    if (user.isEmpty()) {
        return boxes;
    }
    const QDir answerDir( Tf::conf("settings").value("ObjectDetectionAnswerDir").toString() );
    const QDir labelDir(answerDir.filePath(user));

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

    return boxes;
}

bool
AnswerService::writeAreas(const THttpRequest& request, const TSession& session, const QString& labelName, const int& index)
{
    const QStringList images = session.value(kAnswerImagesKey).toStringList();
    if (images.isEmpty() || (index < 0) || (index >= images.count())) {
        return false;
    }

    const QString user = loginUser(session);
    if (user.isEmpty()) {
        return false;
    }
    const QDir answerDir( Tf::conf("settings").value("ObjectDetectionAnswerDir").toString() );
    const QDir labelDir(answerDir.filePath(user)); answerDir.mkpath(user);

    QList<QVariantMap> areas;
    if (Tf::OK != takeAreas(request, areas)) {
        return false;
    }

    const QString labelFileName = QFileInfo(images[index]).completeBaseName() + ".txt";
    // save label with kitti-format.
    QFile label(labelDir.absoluteFilePath(labelFileName));
    if (label.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream stream(&label);
        for (const QVariantMap& v : areas) {
            stream << labelName << " "
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
        return true;
    }

    return false;
}

Tf::HttpStatusCode
AnswerService::takeAreas(const THttpRequest& request, QList<QVariantMap>& areas)
{
    Tf::HttpStatusCode status = Tf::MethodNotAllowed;
    if (Tf::Post == request.method()) {
        if (request.hasJson()) {
            const QJsonArray array = request.jsonData().array();
            for (const auto &v : array) {
                areas.append(v.toObject().toVariantMap());
            }

            // check bounding-box overlapping.
            bool overlapped = false;
            for (int i = 0; i < areas.count(); ++i) {
                for (int j = i + 1; j < areas.count(); ++j) {
                    QRect a(areas[i]["bbox_x"].toFloat(), areas[i]["bbox_y"].toFloat(), areas[i]["bbox_w"].toFloat(), areas[i]["bbox_h"].toFloat());
                    QRect b(areas[j]["bbox_x"].toFloat(), areas[j]["bbox_y"].toFloat(), areas[j]["bbox_w"].toFloat(), areas[j]["bbox_h"].toFloat());
                    if (true == (overlapped = a.intersects(b))) break;
                }
                if (overlapped) break;
            }

            if (! overlapped) {
                status = Tf::OK;
            } else {
                status = Tf::Conflict;
            }
        } else {
            status = Tf::NotAcceptable;
        }
    }
    return status;
}

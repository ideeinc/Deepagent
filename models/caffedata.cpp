#include <TreeFrogModel>
#include "caffedata.h"
#include "caffedataobject.h"

#define STATUS_JSON      "status.json"


QString CaffeData::categoryToLower(CaffeData::Category category)
{
    return QString(QMetaEnum::fromType<CaffeData::Category>().valueToKey((int)category)).toLower();
}

// QString CaffeData::dataTypeToLower(CaffeData::DataType type)
// {
//     return QString(QMetaEnum::fromType<CaffeData::DataType>().valueToKey((int)type)).toLower();
// }

bool CaffeData::writeFile(const QString &filePath, const QByteArray &data, bool overwrite)
{
    QDir dir(QFileInfo(filePath).absolutePath());
    dir.mkpath(".");

    QFile file(filePath);
    if (overwrite || !file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file.write(data);
            file.close();
            return true;
        } else {
            return false;
        }
    }
    return false;
}

QByteArray CaffeData::readFile(const QString &filePath)
{
    QFile file(filePath);
    return (file.open(QIODevice::ReadOnly)) ? file.readAll() : QByteArray();
}

QString CaffeData::dirPath() const
{
    return Tf::app()->webRootPath() % "caffemodel/" % id() % "/";
}

QString CaffeData::jsonFilePath() const
{
    return dirPath() % STATUS_JSON;
}

CaffeData::CaffeData()
    : TAbstractModel(), d(new CaffeDataObject())
{ }

CaffeData::CaffeData(const CaffeData &other)
    : TAbstractModel(), d(new CaffeDataObject(*other.d))
{ }

// CaffeData::CaffeData(const CaffeDataObject &object)
//     : TAbstractModel(), d(new CaffeDataObject(object))
// { }

CaffeData::~CaffeData()
{
    // If the reference count becomes 0,
    // the shared data object 'CaffeDataObject' is deleted.
}

QString CaffeData::id() const
{
    return d->id;
}

void CaffeData::setId(const QString &id)
{
    d->id = id;
}

QString CaffeData::category() const
{
    return d->category;
}

void CaffeData::setCategory(const QString &category)
{
    d->category = category;
}

QString CaffeData::dataType() const
{
    return d->data_type;
}

void CaffeData::setDataType(const QString &dataType)
{
    d->data_type = dataType;
}

QString CaffeData::name() const
{
    return d->name;
}

void CaffeData::setName(const QString &name)
{
    d->name = name;
}

QString CaffeData::username() const
{
    return d->username;
}

void CaffeData::setUsername(const QString &username)
{
    d->username = username;
}

QString CaffeData::state() const
{
    return d->state;
}

void CaffeData::setState(const QString &state)
{
    d->state = state;
}

QString CaffeData::note() const
{
    return d->note;
}

void CaffeData::setNote(const QString &note)
{
    d->note = note;
}

QString CaffeData::caffeVersion() const
{
    return d->caffe_version;
}

void CaffeData::setCaffeVersion(const QString &caffeVersion)
{
    d->caffe_version = caffeVersion;
}

QString CaffeData::createdAt() const
{
    return d->created_at;
}

QString CaffeData::completedAt() const
{
    return d->completed_at;
}

void CaffeData::setCompletedAt(const QString &completedAt)
{
    d->completed_at = completedAt;
}

CaffeData &CaffeData::operator=(const CaffeData &other)
{
    d = other.d;  // increments the reference count of the data
    return *this;
}

/*
CaffeData CaffeData::create(const QString &id, const QString &category, const QString &dataType, const QString &name, const QString &username, const QString &state, const QString &note, const QString &caffeVersion, const QString &completedAt)
{
    CaffeDataObject obj;
    obj.id = id;
    obj.category = category;
    obj.data_type = dataType;
    obj.name = name;
    obj.username = username;
    obj.state = state;
    obj.note = note;
    obj.caffe_version = caffeVersion;
    obj.completed_at = completedAt;
    if (!obj.create()) {
        return CaffeData();
    }
    return CaffeData(obj);
}

CaffeData CaffeData::create(const QVariantMap &values)
{
    CaffeData model;
    model.setProperties(values);
    if (!model.d->create()) {
        model.d->clear();
    }
    return model;
}

CaffeData CaffeData::get(const QString &id)
{
    TSqlORMapper<CaffeDataObject> mapper;
    return CaffeData(mapper.findByPrimaryKey(id));
}

int CaffeData::count()
{
    TSqlORMapper<CaffeDataObject> mapper;
    return mapper.findCount();
}

QList<CaffeData> CaffeData::getAll()
{
    return tfGetModelListByCriteria<CaffeData, CaffeDataObject>(TCriteria());
}

QJsonArray CaffeData::getAllJson()
{
    QJsonArray array;
    TSqlORMapper<CaffeDataObject> mapper;

    if (mapper.find() > 0) {
        for (TSqlORMapperIterator<CaffeDataObject> i(mapper); i.hasNext(); ) {
            array.append(QJsonValue(QJsonObject::fromVariantMap(CaffeData(i.next()).toVariantMap())));
        }
    }
    return array;
}
*/
bool CaffeData::remove()
{
    if (id().isEmpty()) {
        return false;
    }

    auto statusJson = readJson(jsonFilePath());
    auto state = statusJson.value("state").toString();
    if (state.toLower() != "done") {
        tWarn() << "Remove running model";
    }

    QDir subdir(dirPath());
    auto files = subdir.entryInfoList(QDir::Files);
    for (auto f : files) {
        QFile(f.absoluteFilePath()).remove();
    }
    subdir.rmpath(".");
    tDebug() << "removed " << id();
    clear();
    return true;
}

void CaffeData::clear()
{
    d->id.clear();
    return d->clear();
}

bool CaffeData::isNull() const
{
    return d->id.isEmpty();
}

bool CaffeData::writeJson(const QString &filePath, const QVariantMap &values, bool overwrite)
{
    return writeFile(filePath, QJsonDocument(QJsonObject::fromVariantMap(values)).toJson(), overwrite);
}

QVariantMap CaffeData::readJson(const QString &filePath)
{
    QByteArray json = readFile(filePath);
    return (json.isEmpty()) ? QVariantMap() : QJsonDocument::fromJson(json).object().toVariantMap();
}


TModelObject *CaffeData::modelData()
{
    return d.data();
}

const TModelObject *CaffeData::modelData() const
{
    return d.data();
}

QVariantMap CaffeData::toVariantMap() const
{
    QVariantMap ret;

    QVariantMap map = d.data()->toVariantMap();
    for (QMapIterator<QString, QVariant> it(map); it.hasNext(); ) {
        it.next();
        ret.insert(fieldNameToVariableName(it.key()), it.value());
    }

    map.remove("id");
    return ret;
}

void CaffeData::setProperties(const QVariantMap &properties)
{
    // Creates a map of the original property name and the converted name
    const QStringList soprops = d.data()->propertyNames();
    QMap<QString, QString> sopropMap;
    for (auto &orig : soprops) {
        sopropMap.insert(fieldNameToVariableName(orig), orig);
    }

    QVariantMap props;
    for (QMapIterator<QString, QVariant> it(properties); it.hasNext(); ) {
        it.next();
        const QString &p = sopropMap[it.key()];
        if (!p.isEmpty()) {
            props.insert(p, it.value());
        }
    }

    props.remove("id");
    d.data()->setProperties(props);
}


QDataStream &operator<<(QDataStream &ds, const CaffeData &model)
{
    auto varmap = model.toVariantMap();
    ds << varmap;
    return ds;
}

QDataStream &operator>>(QDataStream &ds, CaffeData &model)
{
    QVariantMap varmap;
    ds >> varmap;
    model.setProperties(varmap);
    return ds;
}

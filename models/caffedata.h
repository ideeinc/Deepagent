#ifndef CAFFEDATA_H
#define CAFFEDATA_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QDir>
#include <QSharedDataPointer>
#include <TGlobal>
#include <TAbstractModel>
#include <typeinfo>
#include <cxxabi.h>
#include <cstdlib>

class TModelObject;
class CaffeDataObject;
class QJsonArray;


class T_MODEL_EXPORT CaffeData : public TAbstractModel
{
    Q_GADGET
public:
    enum Category {
        Classify = 0,
        Detection,
        Any,
    };
    Q_ENUM(Category)

    // enum class DataType {
    //     Dataset = 0,
    //     Model,
    //     PretrainedModel,
    // };
    // Q_ENUM(DataType)

    CaffeData();
    CaffeData(const CaffeData &other);
    //CaffeData(const CaffeDataObject &object);
    virtual ~CaffeData();

    QString id() const;
    QString category() const;
    void setCategory(const QString &category);
    QString dataType() const;
    void setDataType(const QString &dataType);
    QString name() const;
    void setName(const QString &name);
    QString username() const;
    void setUsername(const QString &username);
    QString state() const;
    void setState(const QString &state);
    QString note() const;
    void setNote(const QString &note);
    QString caffeVersion() const;
    void setCaffeVersion(const QString &caffeVersion);
    QString createdAt() const;
    QString completedAt() const;
    void setCompletedAt(const QString &completedAt);
    CaffeData &operator=(const CaffeData &other);

    virtual bool create() override = 0;
    virtual bool update() override { return false; }
    virtual bool save()   override { return false; }
    virtual bool remove() override;
    virtual void setProperties(const QVariantMap &properties);
    virtual QVariantMap toVariantMap() const;
    virtual void clear();
    virtual bool isNull() const override;

    //static CaffeData create(const QString &id, const QString &category, const QString &dataType, const QString &name, const QString &username, const QString &state, const QString &note, const QString &caffeVersion, const QString &completedAt);
    //static CaffeData create(const QVariantMap &values);
    //static CaffeData get(const QString &id);
    //static int count();
    //static QList<CaffeData> getAll();
    //static QJsonArray getAllJson();
    static bool writeFile(const QString &filePath, const QByteArray &data, bool overwrite = false);
    static QByteArray readFile(const QString &filePath);
    static bool writeJson(const QString &filePath, const QVariantMap &values, bool overwrite = false);
    static QVariantMap readJson(const QString &filePath);
    static QString categoryToLower(Category category);
    //static QString dataTypeToLower(DataType type);

protected:
    void setId(const QString &id);
    QString dirPath() const;
    QString jsonFilePath() const;
    QVariantMap toBaseMap() const;
    void setBaseProperties(const QVariantMap &properties);
    template <typename T> static T get(const QString &id);
    template <typename T> static QList<T> getAll();

    QSharedDataPointer<CaffeDataObject> d;

    TModelObject *modelData() override;
    const TModelObject *modelData() const override;
    friend QDataStream &operator<<(QDataStream &ds, const CaffeData &model);
    friend QDataStream &operator>>(QDataStream &ds, CaffeData &model);
};


template <typename T>
inline T CaffeData::get(const QString &id)
{
    T model;
    model.setId(id);
    QDir dir(model.dirPath());

    int status = 0;
    auto clsname = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    const QString name(clsname);
    std::free(clsname);

    if (dir.exists()) {
        auto values = readJson(model.jsonFilePath());
        if (! values.isEmpty() && values.value("dataType").toString().toLower() == name.toLower()) {
            model.setProperties(values);
            return model;
        }
    }
    return T();
}


template <typename T>
inline QList<T> CaffeData::getAll()
{
    QList<T> modelList;
    QDir baseDir(Tf::app()->webRootPath() + "caffemodel/");
    const auto dirs = baseDir.entryInfoList(QDir::AllDirs);

    for (auto &d : dirs) {
        auto model = T::get(d.fileName());
        if (! model.isNull()) {
            modelList << model;
        }
    }
    return modelList;

}

#endif // CAFFEDATA_H
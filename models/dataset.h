#ifndef DATASET_H
#define DATASET_H

#include <QStringList>
#include <QDateTime>
#include <QVariant>
#include <QSharedDataPointer>
#include <TGlobal>
#include "caffedata.h"

class TModelObject;
class DatasetObject;
class QJsonArray;


class T_MODEL_EXPORT Dataset : public CaffeData
{
public:
    Dataset();
    Dataset(const Dataset &other);
    ~Dataset();

    int imageWidth() const;
    void setImageWidth(int imageWidth);
    int imageHeight() const;
    void setImageHeight(int imageHeight);
    QString meanFile() const;
    QString meanFilePath() const { return dirPath() + meanFile(); }
    QString meanData() const;
    void setMeanFile(const QString &meanFile);
    QString labelFile() const;
    QString labelFilePath() const { return dirPath() + labelFile(); }
    QString labelData() const;
    void setLabelFile(const QString &labelFile);
    QString trainDbPath() const;
    void setTrainDbPath(const QString &trainDbPath);
    QString valDbPath() const;
    void setValDbPath(const QString &valDbPath);
    QString logFile() const;
    void setLogFile(const QString &logFile);

    // For detection
    int numClasses() const;
    void setNumClasses(int numClasses);
    int numTrainImage() const;
    void setNumTrainImage(int numTrainImage);
    int numValImage() const;
    void setNumValImage(int numValImage);
    QString nameSizeFile() const;
    void setNameSizeFile(const QString &nameSizeFile);
    QString labelMapFile() const;
    void setLabelMapFile(const QString &labelMapFile);
    Dataset &operator=(const Dataset &other);

    bool create() override;
    bool update() override;
    bool save()   override;
    void setProperties(const QVariantMap &properties) override;
    QVariantMap toVariantMap() const override;
    void clear() override;

    static Dataset create(const QVariantMap &values);
    static Dataset get(const QString &id);
    static int count();
    static QList<Dataset> getAll();

private:
    QSharedDataPointer<DatasetObject> d;

    TModelObject *modelData() override;
    const TModelObject *modelData() const override;
    friend class CaffeData;
    friend QDataStream &operator<<(QDataStream &ds, const Dataset &model);
    friend QDataStream &operator>>(QDataStream &ds, Dataset &model);
};

Q_DECLARE_METATYPE(Dataset)
Q_DECLARE_METATYPE(QList<Dataset>)

#endif // DATASET_H

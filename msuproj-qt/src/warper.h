#ifndef WARPER_H
#define WARPER_H

#include <QObject>
#include <msuproj.h>


Q_DECLARE_METATYPE(msumr::RETURN_CODE)

class Warper : public QObject
{
    Q_OBJECT

public:

    explicit Warper(QObject *parent = 0);

    msumr::RETURN_CODE setSrc(const QString &file);
    int getSrcXSize() const;
    int getSrcYSize() const;

    msumr::RETURN_CODE readGCP(const QString &file);
    int getGCPXSize() const;
    int getGCPYSize() const;
    int getGCPXStep() const;
    int getGCPYStep() const;
    QString getUTM() const;

    void setDst(const QString &file);

    void setUseUTM(bool useUTM);
    void setZerosAsND(bool zerosAsND);

    bool ifAddLogo() const;
    void setAddLogo(bool enable);

    unsigned int *getProgressMaxPtr();
    unsigned int *getProgressValPtr();

signals:

    void started();
    void finished(msumr::RETURN_CODE code);

public slots:

    void start();

private:

    bool mUseUTM;
    bool mZerosAsND;
    bool mAddLogo;
    msumr::MSUProj mMSUProj;

};

#endif // WARPER_H

#include <QApplication>

#include <warper.h>

Warper::Warper(QObject *parent) :
    QObject(parent),
    mUseUTM(false),
    mZerosAsND(false),
    mAddLogo(true)
{
    qRegisterMetaType<msumr::RETURN_CODE>("msumr::RETURN_CODE");
}

msumr::RETURN_CODE Warper::setSrc(const QString &file)
{
    return mMSUProj.setSrc(file.toStdString());
}

int Warper::getSrcXSize() const
{
    return mMSUProj.srcXSize();
}

int Warper::getSrcYSize() const
{
    return mMSUProj.srcYSize();
}

msumr::RETURN_CODE Warper::readGCP(const QString &file)
{
    return mMSUProj.readGCP(file.toStdString());
}

int Warper::getGCPXSize() const
{
    return mMSUProj.gcpGrid()->xSize();
}

int Warper::getGCPYSize() const
{
    return mMSUProj.gcpGrid()->ySize();
}

int Warper::getGCPXStep() const
{
    return (int)((double)mMSUProj.srcXSize() / (double)mMSUProj.gcpGrid()->xSize() + 0.5);
}

int Warper::getGCPYStep() const
{
    return (int)((double)mMSUProj.srcYSize() / (double)mMSUProj.gcpGrid()->ySize() + 0.5);
}

QString Warper::getUTM() const
{
    return mMSUProj.gcpGrid()->utmZoneName().c_str();
}

void Warper::setDst(const QString &file)
{
    mMSUProj.setDst(file.toStdString());
}

void Warper::setZerosAsND(bool zerosAsND)
{
    mZerosAsND = zerosAsND;
}

void Warper::setUseUTM(bool useUTM)
{
    mUseUTM = useUTM;
}

bool Warper::ifAddLogo() const
{
    return mAddLogo;
}

void Warper::setAddLogo(bool enable)
{
    mAddLogo = enable;
}

unsigned int *Warper::getProgressMaxPtr()
{
    return mMSUProj.progressMaxPtr();
}

unsigned int *Warper::getProgressValPtr()
{
    return mMSUProj.progressValPtr();
}

void Warper::start()
{
    mMSUProj.setAddLogo(mAddLogo);
    emit started();
    msumr::RETURN_CODE code = mMSUProj.warp(mUseUTM, mZerosAsND);
    this->moveToThread(qApp->thread());
    emit finished(code);
}

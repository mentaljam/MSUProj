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
    return mMSUProj.getSrcXSize();
}

int Warper::getSrcYSize() const
{
    return mMSUProj.getSrcYSize();
}

msumr::RETURN_CODE Warper::readGCP(const QString &file)
{
    return mMSUProj.readGCP(file.toStdString());
}

int Warper::getGCPXSize() const
{
    return mMSUProj.getGCPXSize();
}

int Warper::getGCPYSize() const
{
    return mMSUProj.getGCPYSize();
}

int Warper::getGCPXStep() const
{
    return mMSUProj.getGCPXStep();
}

int Warper::getGCPYStep() const
{
    return mMSUProj.getGCPYStep();
}

QString Warper::getUTM() const
{
    return mMSUProj.getUTM().c_str();
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

void Warper::start()
{
    mMSUProj.setAddLogo(mAddLogo);
    emit started();
    msumr::RETURN_CODE code = mMSUProj.warp(mUseUTM, mZerosAsND);
    this->moveToThread(qApp->thread());
    emit finished(code);
}


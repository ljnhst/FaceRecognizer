#ifndef CTRACKERSEETA_H
#define CTRACKERSEETA_H

#include <QObject>
#include <QSharedPointer>

#include "Tracker.h"
#include "seeta/FaceTracker.h"

class CTrackerSeeta : public CTracker
{
    Q_OBJECT
public:
    explicit CTrackerSeeta(QObject *parent = nullptr);
    
    virtual int Track(const QImage &image, QVector<strFace> &faces);
    
private:
    virtual int UpdateParameter(QString &szErr);

    QSharedPointer<seeta::FaceTracker> m_Tracker;
    bool m_bInit;    
};

#endif // CTRACKERSEETA_H

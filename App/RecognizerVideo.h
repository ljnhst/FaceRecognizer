#ifndef CRECOGNIZERVIDEO_H
#define CRECOGNIZERVIDEO_H

#pragma once
#include <QObject>
#include "Factory.h"

class CRecognizerVideo : public QObject
{
    Q_OBJECT
public:
    explicit CRecognizerVideo(QObject *parent = nullptr);
    
Q_SIGNALS:
    void sigRecognized(const QMap<int, QString> &faceInfo);

public slots:
    void slotRecognize(const QImage &image, const QVector<CTracker::strFace> &faces);
};

#endif // CRECOGNIZERVIDEO_H

//
// Created by bklytics on 10/11/24.
//

#ifndef TASK_METADATA_H
#define TASK_METADATA_H

#include <QString>

class task_metadata {
public:
    QString imagePath;
    QString caseNumber;
    QString description;
    QString examinerName;
    QString evidenceName;
    QString note;
    QString fileFormat;
    QString averageSpeed;
    QString time;


    void writeTo(QString path);
};



#endif //TASK_METADATA_H

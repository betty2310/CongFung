//
// Created by bklytics on 10/10/24.
//

#ifndef TASK_H
#define TASK_H

#include <QString>

enum HashType {
    MD5,
    SHA1,
    SHA256
};

enum T_CLI {
    DC3DD,
    EWFACQUIRE,
    AFFIMAGER,
};

struct Task {
    QString id;
    QString imageName;
    QString source;
    QString destination;
    HashType hash;
    QString logPath;
    QString mountedPath;
    QString pdNumber;
    T_CLI command;

    QString caseNumber;
    QString description;
    QString examiner;
    QString evidence;
    QString notes;

};


#endif //TASK_H

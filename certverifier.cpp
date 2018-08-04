#include "certverifier.h"
#include <iostream>
#include <QSslCertificate>
#include <QDebug>
//CertVerifier::CertVerifier()
//{

//}
void CertVerifier::verify(vmime::shared_ptr <vmime::security::cert::certificateChain> chain, const vmime::string& hostname){


    vmime::shared_ptr<vmime::security::cert::certificate> cert = chain->getAt(0);
    std::cout<<"Server send a "<<cert->getType()<<" certificate."<<std::endl;

    vmime::byteArray data = cert->getEncoded();

    QSslCertificate qcert(QByteArray(reinterpret_cast<const char*>(data.data()),data.size()));
    if(qcert.isBlacklisted()){
        qDebug()<<"cert is blacklisted";
    }else if(qcert.isSelfSigned()){
        qDebug()<<"cert is selfsigned";
    }


}

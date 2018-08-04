#ifndef CERTVERIFIER_H
#define CERTVERIFIER_H

#include <vmime/security/cert/certificateVerifier.hpp>

class CertVerifier : public vmime::security::cert::certificateVerifier
{
public:
    //CertVerifier();

    void verify(vmime::shared_ptr <vmime::security::cert::certificateChain> chain, const vmime::string& hostname);
};

#endif // CERTVERIFIER_H

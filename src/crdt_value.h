#ifndef CRDT_VALUE_H
#define CRDT_VALUE_H

#include <string>

#include "gcounter.h"
#include "lww.h"
#include "orset.h"

enum class CRDTType {
    GCOUNTER,
    PNCOUNTER,
    LWWREG,
    ORSET
};

inline std::string crdtTypeToString(CRDTType type) {
    switch (type) {
    case CRDTType::GCOUNTER:
        return "GCOUNTER";
    case CRDTType::PNCOUNTER:
        return "PNCOUNTER";
    case CRDTType::LWWREG:
        return "LWWREG";
    case CRDTType::ORSET:
        return "ORSET";
    default:
        return "UNKNOWN";
    }
}

inline bool stringToCRDTType(const std::string& text, CRDTType& type) {
    if (text == "GCOUNTER") {
        type = CRDTType::GCOUNTER;
        return true;
    }

    if (text == "PNCOUNTER") {
        type = CRDTType::PNCOUNTER;
        return true;
    }

    if (text == "LWWREG") {
        type = CRDTType::LWWREG;
        return true;
    }

    if (text == "ORSET") {
        type = CRDTType::ORSET;
        return true;
    }

    return false;
}

struct CRDTValue {
    CRDTType type;

    GCounter gcounter;
    PNCounter pncounter;
    LWWRegister lwwreg;
    ORSet orset;

    CRDTValue(const std::string& nodeId, CRDTType crdtType)
        : type(crdtType),
        gcounter(nodeId),
        pncounter(nodeId),
        lwwreg(nodeId),
        orset(nodeId) {
    }
};

#endif
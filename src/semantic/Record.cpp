#include "semantic/Record.hpp"

#include <iostream>

void Record::printRecord(std::ostream &os) const {
    os << id << "\t" << type << "\t" << getRecord();
}

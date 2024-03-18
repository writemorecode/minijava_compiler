#ifndef RECORD_HPP
#define RECORD_HPP

#include <string>

class Record {
    std::string id, type;

  public:
    Record(const std::string &id, const std::string &type)
        : id{id}, type{type} {}

    virtual ~Record() = default;

    std::string getID() const { return id; }
    std::string getType() const { return type; }
    void printRecord(std::ostream &os) const;

    virtual std::string getRecord() const = 0;
};

#endif

#ifndef RECORD_HPP
#define RECORD_HPP

#include <string>

class Record {
    std::string id, type;

  public:
    Record(std::string id, std::string type)
        : id{std::move(id)}, type{std::move(type)} {}

    virtual ~Record() = default;

    std::string getID() const { return id; }
    std::string getType() const { return type; }
    void printRecord(std::ostream &os) const;

    virtual std::string getRecord() const = 0;
};

#endif

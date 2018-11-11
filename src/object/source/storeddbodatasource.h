#ifndef STOREDDBODATASOURCE_H
#define STOREDDBODATASOURCE_H

#include "configurable.h"

#include <string>

class DBObject;

class StoredDBODataSource : public Configurable
{
public:
    StoredDBODataSource(const std::string& class_id, const std::string& instance_id, DBObject& object);

    unsigned int id() const;

    const std::string &name() const;

    void shortName(const std::string &short_name);
    const std::string &shortName() const;

    void sac(unsigned char sac);
    unsigned char sac() const;

    void sic(unsigned char sic);
    unsigned char sic() const;

    void latitude(double latitiude);
    double latitude() const;

    void longitude(double longitude_);
    double longitude() const;

    void altitude(double altitude);
    double altitude() const;

private:
    DBObject& object_;

    unsigned int id_{0};
    std::string name_;
    std::string short_name_;
    unsigned int sac_;
    unsigned int sic_ {0};
    double latitude_ {0}; //degrees
    double longitude_ {0}; // degrees
    double altitude_ {0};  // meter above msl

protected:
    virtual void checkSubConfigurables () {}
};

#endif // STOREDDBODATASOURCE_H

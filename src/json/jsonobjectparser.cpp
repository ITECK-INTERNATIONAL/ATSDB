#include "jsonobjectparser.h"

#include "buffer.h"
#include "dbobject.h"
#include "unitmanager.h"
#include "unit.h"
#include "dbobjectmanager.h"
#include "atsdb.h"

#include <algorithm>

using namespace nlohmann;

JSONObjectParser::JSONObjectParser (const std::string& class_id, const std::string& instance_id, Configurable* parent)
    :  Configurable (class_id, instance_id, parent)
{
    registerParameter("db_object_name", &db_object_name_, "");

    registerParameter("json_container_key", &json_container_key_, "");
    registerParameter("json_key", &json_key_, "*");
    registerParameter("json_value", &json_value_, "");

    registerParameter("override_key_variable", &override_key_variable_, false);

    registerParameter("override_data_source", &override_data_source_, false);
    registerParameter("data_source_variable_name", &data_source_variable_name_, "");

    assert (db_object_name_.size());

    createSubConfigurables ();
}

JSONObjectParser& JSONObjectParser::operator=(JSONObjectParser&& other)
{
    db_object_name_ = other.db_object_name_;
    db_object_ = other.db_object_;

    json_container_key_ = other.json_container_key_;
    json_key_ = other.json_key_;
    json_value_ = other.json_value_;

    data_mappings_ = std::move(other.data_mappings_);

    var_list_ = other.var_list_;

    override_key_variable_ = other.override_key_variable_;
    has_key_mapping_ = other.has_key_mapping_;
    has_key_variable_ = other.has_key_variable_;

    override_data_source_ = other.override_data_source_;
    data_source_variable_name_ = other.data_source_variable_name_;

    initialized_ = other.initialized_;
    key_variable_ = other.key_variable_;

    not_parse_all_ = other.not_parse_all_;

    list_ = other.list_;

    other.configuration().updateParameterPointer ("db_object_name", &db_object_name_);
    other.configuration().updateParameterPointer ("json_key", &json_key_);
    other.configuration().updateParameterPointer ("json_value", &json_value_);
    other.configuration().updateParameterPointer ("override_key_variable", &override_key_variable_);
    other.configuration().updateParameterPointer ("override_data_source", &override_data_source_);

    widget_ = std::move(other.widget_);
    if (widget_)
        widget_->setParser(*this);
    other.widget_ = nullptr;

    return static_cast<JSONObjectParser&>(Configurable::operator=(std::move(other)));
}

void JSONObjectParser::generateSubConfigurable (const std::string& class_id, const std::string& instance_id)
{
    if (class_id == "JSONDataMapping")
    {
//        unsigned int id = configuration().getSubConfiguration(
//                    class_id, instance_id).getParameterConfigValueUint("id");

//        assert (stored_data_sources_.find (id) == stored_data_sources_.end());

//        loginf << "DBObject: generateSubConfigurable: generating stored DS " << instance_id << " with id " << id;

//        stored_data_sources_.emplace(std::piecewise_construct,
//                                     std::forward_as_tuple(id),  // args for key
//                                     std::forward_as_tuple(class_id, instance_id, this));  // args for mapped value

        data_mappings_.emplace_back (class_id, instance_id, *this);
    }
    else
        throw std::runtime_error ("DBObject: generateSubConfigurable: unknown class_id "+class_id );
}

DBObject& JSONObjectParser::dbObject() const
{
    assert (db_object_);
    return *db_object_;
}

std::string JSONObjectParser::JSONKey() const
{
    return json_key_;
}

void JSONObjectParser::JSONKey(const std::string& json_key)
{
    loginf << "JSONObjectParser: JSONKey: " << json_key;

    json_key_ = json_key;
}

std::string JSONObjectParser::JSONValue() const
{
    return json_value_;
}

void JSONObjectParser::JSONValue(const std::string& json_value)
{
    loginf << "JSONObjectParser: JSONValue: " << json_value;

    json_value_ = json_value;
}

std::string JSONObjectParser::JSONContainerKey() const
{
    return json_container_key_;
}

void JSONObjectParser::JSONContainerKey(const std::string& key)
{
    loginf << "JSONObjectParser: JSONContainerKey: " << key;

    json_container_key_ = key;
}
//void JSONObjectParser::addMapping (JSONDataMapping mapping)
//{
//    assert (mapping.variable().hasCurrentDBColumn());

//    data_mappings_.push_back(mapping);
//    list_.addProperty(mapping.variable().name(), mapping.variable().dataType());
//    var_list_.add(mapping.variable());

//    if (mapping.variable().isKey())
//    {
//        assert (mapping.variable().dataType() == PropertyDataType::INT);
//        has_key_mapping_ = true;
//    }
//}

void JSONObjectParser::initialize ()
{
    assert (!db_object_);

    DBObjectManager& obj_man = ATSDB::instance().objectManager();

    if (!obj_man.existsObject(db_object_name_))
        logwrn << "JSONObjectParser: ctor: dbobject '" << db_object_name_ << "' does not exist";
    else
        db_object_ = &obj_man.object(db_object_name_);

    assert (db_object_);

    if (!initialized_)
    {
        for (auto& mapping : data_mappings_)
        {
            mapping.initializeIfRequired();

            assert (mapping.variable().hasCurrentDBColumn());
            list_.addProperty(mapping.variable().name(), mapping.variable().dataType());
            var_list_.add(mapping.variable());

            if (mapping.variable().isKey())
            {
                assert (mapping.variable().dataType() == PropertyDataType::INT);
                has_key_mapping_ = true;
            }
        }

        if (!has_key_mapping_ && db_object_->hasKeyVariable()) // first time only, add key variable
            has_key_variable_ = true;
        else
            has_key_variable_ = has_key_mapping_; // couldn't be added, can only have if mapped one exists

        if (has_key_variable_ && key_variable_ == nullptr)
        {
            key_variable_ = &db_object_->getKeyVariable();
            assert (key_variable_);
            assert (key_variable_->dataType() == PropertyDataType::INT);
            if (!list_.hasProperty(key_variable_->name()))
            {
                list_.addProperty(key_variable_->name(), key_variable_->dataType());
                var_list_.add(*key_variable_);
            }
        }

        if (override_key_variable_ && !has_key_variable_)
        {
            logwrn << "JsonMapping: initialize: override key set but no key variable exists, disabling override";
            override_key_variable_ = false;
        }
        else
        {
            assert (data_source_variable_name_.size());
            assert (db_object_->hasVariable(data_source_variable_name_));

            list_.addProperty(data_source_variable_name_, PropertyDataType::INT);
            var_list_.add(db_object_->variable(data_source_variable_name_));
        }

        not_parse_all_ = (json_key_ != "*") && (json_value_ != "*");

        initialized_ = true;
    }
}

std::shared_ptr<Buffer> JSONObjectParser::getNewBuffer () const
{
    assert (initialized_);
    assert (db_object_);
    return std::shared_ptr<Buffer> {new Buffer (list_, db_object_->name())};
}

unsigned int JSONObjectParser::parseJSON (nlohmann::json& j, std::shared_ptr<Buffer> buffer) const
{
    assert (initialized_);

    assert (buffer != nullptr);

    unsigned int row_cnt = buffer->size();
    unsigned int skipped_cnt = 0;
    unsigned int all_cnt = 0;

    bool skipped;

    if (json_container_key_.size())
    {
        if (j.find(json_container_key_) != j.end())
        {
            json& ac_list = j.at(json_container_key_);
            assert (ac_list.is_array());

            //loginf << "found target report array in '" << json_container_key_  << "', parsing";

            for (auto tr_it = ac_list.begin(); tr_it != ac_list.end(); ++tr_it)
            {
                //logdbg << "new target report";

                json& tr = tr_it.value();
                assert (tr.is_object());

                skipped = parseTargetReport (tr, buffer, row_cnt);

                if (!skipped)
                    ++row_cnt;
                else
                    ++skipped_cnt;

                ++all_cnt;
            }
        }
        else
            loginf << "found target report array '" << json_container_key_  << "' not found";
    }
    else
    {
        //loginf << "found single target report";
        assert (j.is_object());

        skipped = parseTargetReport (j, buffer, row_cnt);

        if (!skipped)
            ++row_cnt;
    }

    return row_cnt;
}

bool JSONObjectParser::parseTargetReport (const nlohmann::json& tr, std::shared_ptr<Buffer> buffer,
                                          size_t row_cnt) const
{
    // check key match
    if (not_parse_all_)
    {
        if (tr.find (json_key_) != tr.end())
        {
            if (tr.at(json_key_) != json_value_)
            {
                logdbg << "JsonMapping: parseTargetReport: skipping because of wrong key value " << tr.at(json_key_);
                return true;
            }
            else
                logdbg << "JsonMapping: parseTargetReport: parsing with correct key and value";
        }
        else
        {
            logdbg << "JsonMapping: parseTargetReport: skipping because of missing key '" << json_key_ << "'";
            return true;
        }
    }

    PropertyDataType data_type;
    std::string current_var_name;

    bool mandatory_missing;

    for (const auto& data_it : data_mappings_)
    {
        //logdbg << "setting data mapping key " << data_it.jsonKey();

        data_type = data_it.variable().dataType();
        current_var_name = data_it.variable().name();

        switch (data_type)
        {
        case PropertyDataType::BOOL:
        {
            logdbg << "bool " << current_var_name << " format '" << data_it.jsonValueFormat() << "'";
            assert (buffer->has<bool>(current_var_name));
            mandatory_missing = data_it.findAndSetValue (tr, buffer->get<bool> (current_var_name), row_cnt);

            break;
        }
        case PropertyDataType::CHAR:
        {
            logdbg << "char " << current_var_name << " format '" << data_it.jsonValueFormat() << "'";
            assert (buffer->has<char>(current_var_name));
            mandatory_missing = data_it.findAndSetValue (tr, buffer->get<char> (current_var_name), row_cnt);

            break;
        }
        case PropertyDataType::UCHAR:
        {
            logdbg << "uchar " << current_var_name << " format '" << data_it.jsonValueFormat() << "'";
            assert (buffer->has<unsigned char>(current_var_name));
            mandatory_missing = data_it.findAndSetValue (tr, buffer->get<unsigned char> (current_var_name), row_cnt);

            break;
        }
        case PropertyDataType::INT:
        {
            logdbg << "int " << current_var_name << " format '" << data_it.jsonValueFormat() << "'";
            assert (buffer->has<int>(current_var_name));
            mandatory_missing = data_it.findAndSetValue (tr, buffer->get<int> (current_var_name), row_cnt);

            break;
        }
        case PropertyDataType::UINT:
        {
            logdbg << "uint " << current_var_name << " format '" << data_it.jsonValueFormat() << "'";
            assert (buffer->has<unsigned int>(current_var_name));
            mandatory_missing = data_it.findAndSetValue (tr, buffer->get<unsigned int> (current_var_name), row_cnt);

            break;
        }
        case PropertyDataType::LONGINT:
        {
            logdbg << "long " << current_var_name << " format '" << data_it.jsonValueFormat() << "'";
            assert (buffer->has<long int>(current_var_name));
            mandatory_missing = data_it.findAndSetValue (tr, buffer->get<long int> (current_var_name), row_cnt);

            break;
        }
        case PropertyDataType::ULONGINT:
        {
            logdbg << "ulong " << current_var_name << " format '" << data_it.jsonValueFormat() << "'";
            assert (buffer->has<unsigned long>(current_var_name));
            mandatory_missing = data_it.findAndSetValue (tr, buffer->get<unsigned long> (current_var_name), row_cnt);

            break;
        }
        case PropertyDataType::FLOAT:
        {
            logdbg << "float " << current_var_name << " format '" << data_it.jsonValueFormat() << "'";
            assert (buffer->has<float>(current_var_name));
            mandatory_missing = data_it.findAndSetValue (tr, buffer->get<float> (current_var_name), row_cnt);
            break;
        }
        case PropertyDataType::DOUBLE:
        {
            logdbg << "double " << current_var_name << " format '" << data_it.jsonValueFormat() << "'";
            assert (buffer->has<double>(current_var_name));
            mandatory_missing = data_it.findAndSetValue (tr, buffer->get<double> (current_var_name), row_cnt);

            break;
        }
        case PropertyDataType::STRING:
        {
            logdbg << "string " << current_var_name << " format '" << data_it.jsonValueFormat() << "'";
            assert (buffer->has<std::string>(current_var_name));
            mandatory_missing = data_it.findAndSetValue (tr, buffer->get<std::string> (current_var_name), row_cnt);

            break;
        }
        default:
            logerr  <<  "JsonMapping: parseTargetReport: impossible for property type "
                     << Property::asString(data_type);
            throw std::runtime_error ("JsonMapping: parseTargetReport: impossible property type "
                                      + Property::asString(data_type));
        }

        if (mandatory_missing)
            break;
    }

    if (mandatory_missing)
    {
        // cleanup
        if (buffer->size() > row_cnt)
            buffer->cutToSize(row_cnt);
    }

    return mandatory_missing;
}

bool JSONObjectParser::hasMapping (unsigned int index) const
{
    return index < data_mappings_.size();
}
void JSONObjectParser::removeMapping (unsigned int index)
{
    assert (hasMapping(index));
    data_mappings_.erase(data_mappings_.begin()+index);
}

void JSONObjectParser::transformBuffer (std::shared_ptr<Buffer> buffer, long key_begin) const
{
    assert (db_object_);

    loginf << "JsonMapping: transformBuffer: object " << db_object_->name();

    if (override_data_source_)
    {
        loginf << "JsonMapping: transformBuffer: overiding data source for object " << db_object_->name()
               << " ds id name '" << data_source_variable_name_ << "'";
        assert (data_source_variable_name_.size());
        assert (buffer->has<int>(data_source_variable_name_));

        //buffer->addProperty(data_source_variable_name_, PropertyDataType::INT);
        NullableVector<int>& ds_id_vector = buffer->get<int> (data_source_variable_name_);
        size_t size = buffer->size();

        for (size_t cnt=0; cnt < size; ++cnt)
            ds_id_vector.set(cnt, 0);
    }

    if (override_key_variable_)
    {
        loginf << "JsonMapping: transformBuffer: overiding key variable for object " << db_object_->name();
        assert (key_begin >= 0);
        size_t key_cnt = key_begin;
        assert (key_variable_);
        assert (buffer->has<int>(key_variable_->name()));
        NullableVector<int>& key_vector = buffer->get<int> (key_variable_->name());
        size_t size = buffer->size();

        for (size_t cnt=0; cnt < size; ++cnt)
            key_vector.set(cnt, key_cnt++);
    }

    for (auto& data_it : data_mappings_)
    {
        if (data_it.dimension() != data_it.variable().dimension())
            logwrn << "JsonMapping: transformBuffer: variable " << data_it.variable().name()
                   << " has differing dimensions " << data_it.dimension() << " " << data_it.variable().dimension();
        else if (data_it.unit() != data_it.variable().unit()) // do unit conversion stuff
        {
            logdbg << "JsonMapping: transformBuffer: variable " << data_it.variable().name()
                   << " of same dimension has different units " << data_it.unit() << " " << data_it.variable().unit();

            const Dimension &dimension = UnitManager::instance().dimension (data_it.variable().dimension());
            double factor;

            factor = dimension.getFactor (data_it.unit(), data_it.variable().unit());
            std::string current_var_name = data_it.variable().name();
            PropertyDataType data_type = data_it.variable().dataType();

            logdbg  << "JsonMapping: transformBuffer: correct unit transformation with factor " << factor;

            switch (data_type)
            {
            case PropertyDataType::BOOL:
            {
                assert (buffer->has<bool>(current_var_name));
                NullableVector<bool> &array_list = buffer->get<bool>(current_var_name);
                logwrn << "JsonMapping: transformBuffer: double multiplication of boolean variable "
                       << current_var_name;
                array_list *= factor;
                break;
            }
            case PropertyDataType::CHAR:
            {
                assert (buffer->has<char>(current_var_name));
                NullableVector<char> &array_list = buffer->get<char> (current_var_name);
                logwrn << "JsonMapping: transformBuffer: double multiplication of char variable "
                       << current_var_name;
                array_list *= factor;
                break;
            }
            case PropertyDataType::UCHAR:
            {
                assert (buffer->has<unsigned char>(current_var_name));
                NullableVector<unsigned char> &array_list = buffer->get<unsigned char> (current_var_name);
                logwrn << "JsonMapping: transformBuffer: double multiplication of unsigned char variable "
                       << current_var_name;
                array_list *= factor;
                break;
            }
            case PropertyDataType::INT:
            {
                assert (buffer->has<int>(current_var_name));
                NullableVector<int> &array_list = buffer->get<int> (current_var_name);
                array_list *= factor;
                break;
            }
            case PropertyDataType::UINT:
            {
                assert (buffer->has<unsigned int>(current_var_name));
                NullableVector<unsigned int> &array_list = buffer->get<unsigned int> (current_var_name);
                array_list *= factor;
                break;
            }
            case PropertyDataType::LONGINT:
            {
                assert (buffer->has<long int>(current_var_name));
                NullableVector<long int> &array_list = buffer->get<long int>(current_var_name);
                array_list *= factor;
                break;
            }
            case PropertyDataType::ULONGINT:
            {
                assert (buffer->has<unsigned long>(current_var_name));
                NullableVector<unsigned long> &array_list = buffer->get<unsigned long>(current_var_name);
                array_list *= factor;
                break;
            }
            case PropertyDataType::FLOAT:
            {
                assert (buffer->has<float>(current_var_name));
                NullableVector<float> &array_list = buffer->get<float>(current_var_name);
                array_list *= factor;
                break;
            }
            case PropertyDataType::DOUBLE:
            {
                assert (buffer->has<double>(current_var_name));
                NullableVector<double> &array_list = buffer->get<double>(current_var_name);
                array_list *= factor;
                break;
            }
            case PropertyDataType::STRING:
                logerr << "JsonMapping: transformBuffer: unit transformation for string variable "
                       << data_it.variable().name() << " impossible";
                break;
            default:
                logerr  <<  "JsonMapping: transformBuffer: unknown property type "
                         << Property::asString(data_type);
                throw std::runtime_error ("JsonMapping: transformBuffer: unknown property type "
                                          + Property::asString(data_type));
            }
        }
    }
}

bool JSONObjectParser::overrideKeyVariable() const
{
    return override_key_variable_;
}

void JSONObjectParser::overrideKeyVariable(bool override)
{
    loginf << "JSONObjectParser: overrideKeyVariable: " << override;

    override_key_variable_ = override;
}

const DBOVariableSet& JSONObjectParser::variableList() const
{
    return var_list_;
}

bool JSONObjectParser::overrideDataSource() const
{
    return override_data_source_;
}

void JSONObjectParser::overrideDataSource(bool override)
{
    loginf << "JSONObjectParser: overrideDataSource: " << override;
    override_data_source_ = override;
}

std::string JSONObjectParser::dataSourceVariableName() const
{

    return data_source_variable_name_;
}

void JSONObjectParser::dataSourceVariableName(const std::string& name)
{
    loginf << "JSONObjectParser: dataSourceVariableName: " << name;

    data_source_variable_name_ = name;
}

JSONObjectParserWidget* JSONObjectParser::widget ()
{
    if (!widget_)
    {
        widget_.reset(new JSONObjectParserWidget (*this));
        assert (widget_);
    }

    return widget_.get(); // needed for qt integration, not pretty
}

std::string JSONObjectParser::dbObjectName() const
{
    return db_object_name_;
}



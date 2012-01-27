#ifndef DETDESC_IDDESCRIPTOR_H
#define DETDESC_IDDESCRIPTOR_H

#include <string>
#include <vector>
#include <map>


/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  void split(const std::string& str, char delim, std::vector<std::string>& result);

  class IDDescriptor  {
  public:
    typedef std::pair<int,int>          Field;
    typedef std::map<std::string,Field> FieldMap;
    typedef std::map<int,std::string>   FieldIDs;
  protected:
    FieldMap    m_fieldMap;
    FieldIDs    m_fieldIDs;
    int         m_maxBit;
  public:
    IDDescriptor();
    IDDescriptor(const std::string& description);
    virtual ~IDDescriptor();
    void construct(const std::string& description);
    int maxBit() const { return m_maxBit; }
    const FieldIDs& ids() const    { return m_fieldIDs; }
    const FieldMap& fields() const { return m_fieldMap; }
  };

}         /* End namespace DetDesc   */
#endif    /* DETDESC_IDDESCRIPTOR_H     */

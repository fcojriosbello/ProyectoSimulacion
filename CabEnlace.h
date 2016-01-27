/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

using namespace ns3;

#include "ns3/header.h"

/* A la trama original solo le añadiremos un nuevo octeto que será el 
   ID de la estación que le toque transmitir.

   |--------------|------------|
   |    TOKEN_ID  |   Trama    |
   |--------------|------------|
    */

class CabEnlace : public Header
{
public:
  virtual TypeId   GetInstanceTypeId (void) const           { return GetTypeId (); }

  virtual void     Print (std::ostream &os) const
  {
    os << "Token ID = " << m_tokenID;
  }

  virtual uint32_t GetSerializedSize (void) const           { return 2; }

  virtual void     Serialize (Buffer::Iterator contenedor) const
  {
    contenedor.WriteU8 (m_tokenID);
  }

  virtual uint32_t Deserialize (Buffer::Iterator contenedor)
  {
    m_tokenID = contenedor.ReadU8 ();
    return 1;
  }

  void     SetData (uint8_t tokenID)
  {
    m_tokenID = tokenID;
  }

  uint8_t  GetTokenID (void) const { return m_tokenID; }


private:
  uint8_t m_tokenID;
};

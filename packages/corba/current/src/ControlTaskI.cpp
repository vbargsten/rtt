// -*- C++ -*-
//
// $Id$

// ****  Code generated by the The ACE ORB (TAO) IDL Compiler ****
// TAO and the TAO IDL Compiler have been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// and
//       Distributed Object Computing Laboratory
//       University of California at Irvine
//       Irvine, CA
//       USA
//       http://doc.ece.uci.edu/
// and
//       Institute for Software Integrated Systems
//       Vanderbilt University
//       Nashville, TN
//       USA
//       http://www.isis.vanderbilt.edu/
//
// Information about TAO is available at:
//     http://www.cs.wustl.edu/~schmidt/TAO.html

// TAO_IDL - Generated from 
// be/be_codegen.cpp:910

#include "rtt/corba/ControlTaskI.h"
#include "rtt/corba/ControlTaskServer.hpp"
#include "rtt/corba/ControlTaskProxy.hpp"
#include "rtt/corba/CosPropertyServiceI.h"
#include "rtt/corba/AttributesI.h"
#include "rtt/corba/FactoriesI.h"
#include "rtt/corba/ScriptingAccessI.h"
#include "rtt/Method.hpp"



using namespace RTT;
using namespace RTT::Corba;
using namespace CosPropertyService;

// ControlObject:
Orocos_ControlObject_i::Orocos_ControlObject_i (RTT::OperationInterface* orig )
    : mobj( orig ), mMFact(), mCFact()
{
}

// Implementation skeleton destructor
Orocos_ControlObject_i::~Orocos_ControlObject_i (void)
{
}

 char* Orocos_ControlObject_i::getName (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) 
{
    CORBA::String_var ret = CORBA::string_dup( mobj->getName().c_str() );
    return ret._retn();
}

 char* Orocos_ControlObject_i::getDescription (
      
    )
    ACE_THROW_SPEC ((
      CORBA::SystemException
      )) 
{
    CORBA::String_var ret = CORBA::string_dup( mobj->getDescription().c_str() );
    return ret._retn();
}

::Orocos::MethodInterface_ptr Orocos_ControlObject_i::methods (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    if ( CORBA::is_nil( mMFact ) ) {
        Logger::log() << Logger::Info << "Creating MethodInterface."<<Logger::endl;
        Orocos_MethodInterface_i* mserv = new Orocos_MethodInterface_i( mobj->methods() );
        mMFact = mserv->_this();
    }
    return Orocos::MethodInterface::_duplicate( mMFact.in() );
}

::Orocos::CommandInterface_ptr Orocos_ControlObject_i::commands (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    if ( CORBA::is_nil( mCFact ) ) {
        Logger::log() << Logger::Info << "Creating CommandInterface."<<Logger::endl;
        Orocos_CommandInterface_i* mserv = new Orocos_CommandInterface_i( mobj->commands() );
        mCFact = mserv->_this();
    }
    return Orocos::CommandInterface::_duplicate( mCFact.in() );
}



// Implementation skeleton constructor
Orocos_ControlTask_i::Orocos_ControlTask_i (TaskContext* orig)
    : Orocos_ControlObject_i(orig), mtask( orig ), mCosProps( ), mEEFact()
{
    // Add the corba object to the interface:
    mtask->methods()->addMethod(method("shutdown", &Orocos_ControlTask_i::shutdownCORBA, this),
                                   "Shutdown CORBA ORB. This function makes RunOrb() return.");

}

  void Orocos_ControlTask_i::shutdownCORBA() {
	  ControlTaskServer::ShutdownOrb(false);
  }


// Implementation skeleton destructor
Orocos_ControlTask_i::~Orocos_ControlTask_i (void)
{
}

::CosPropertyService::PropertySet_ptr Orocos_ControlTask_i::propertySet (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    // create and activate servant.
    if ( CORBA::is_nil( mCosProps) ) {
        Logger::log() << Logger::Info << "Creating PropertySet."<<Logger::endl;
        CosPropertyService_PropertySet_i* props = new CosPropertyService_PropertySet_i( mtask->attributes()->properties() );
        mCosProps = props->_this();
    }
    return PropertySet::_duplicate( mCosProps.in() );
}

::Orocos::AttributeInterface_ptr Orocos_ControlTask_i::attributes (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    if ( CORBA::is_nil( mAttrs) ) {
        Logger::log() << Logger::Info << "Creating AttributeInterface."<<Logger::endl;
        Orocos_AttributeInterface_i* attrs = new Orocos_AttributeInterface_i( mtask->attributes() );
        mAttrs = attrs->_this();
    }
    return Orocos::AttributeInterface::_duplicate( mAttrs.in() );
#if 0
    // create and activate servant.
    if ( CORBA::is_nil( mCosAttrs) ) {
        Logger::log() << Logger::Info << "Creating Attributes PropertySet."<<Logger::endl;
        CosPropertyService_PropertySet_i* props = new CosPropertyService_PropertySet_i( mtask->attributes() );
        mCosProps = props->_this();
    }
    return PropertySet::_duplicate( mCosProps.in() );
#endif
}


::Orocos::ScriptingAccess_ptr Orocos_ControlTask_i::scripting (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    if ( CORBA::is_nil( mEEFact ) ) {
        Logger::log() << Logger::Info << "Creating ScriptingAccess."<<Logger::endl;
        Orocos_ScriptingAccess_i* mserv = new Orocos_ScriptingAccess_i( mtask->scripting() );
        mEEFact = mserv->_this();
    }
    return Orocos::ScriptingAccess::_duplicate( mEEFact.in() );
}


::Orocos::ControlTask::ControlTaskNames * Orocos_ControlTask_i::getPeerList (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    TaskContext::PeerList peers = mtask->getPeerList();
    ::Orocos::ControlTask::ControlTaskNames_var result = new ::Orocos::ControlTask::ControlTaskNames();
    result->length( peers.size() );
    for (unsigned int i=0; i != peers.size(); ++i )
        result[i] = CORBA::string_dup( peers[i].c_str() );
    
    return result._retn();
}

::Orocos::ControlObject_ptr Orocos_ControlTask_i::getObject (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    std::string pname(name);
    OperationInterface* task = mtask->getObject( pname );
    if ( task ) {
        // create or lookup new server for this object.
        Orocos_ControlObject_i* ret = new Orocos_ControlObject_i(task);
        return ret->_this();
    }
    return 0;
}


::Orocos::ObjectList * Orocos_ControlTask_i::getObjectList (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    TaskContext::ObjectList objects = mtask->getObjectList();
    ::Orocos::ObjectList_var result = new ::Orocos::ObjectList();
    result->length( objects.size() );
    for (unsigned int i=0; i != objects.size(); ++i )
        result[i] = CORBA::string_dup( objects[i].c_str() );
    
    return result._retn();
}

CORBA::Boolean Orocos_ControlTask_i::hasObject (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    std::string mname(name);
    return mtask->getObject(mname);
}


::Orocos::ControlTask_ptr Orocos_ControlTask_i::getPeer (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    std::string pname(name);
    TaskContext* task = mtask->getPeer( pname );
    if ( task ) {
        // create or lookup new server for this peer.
        return ControlTaskServer::Create( task )->server();
    }
    return 0;
}

CORBA::Boolean Orocos_ControlTask_i::addPeer (
    ::Orocos::ControlTask_ptr p,
    const char * alias
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    std::string malias(alias);
    if (mtask->hasPeer(alias) == false )
        return mtask->addPeer( ControlTaskProxy::Create(p), alias );
    return false;
}

CORBA::Boolean Orocos_ControlTask_i::hasPeer (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    std::string mname(name);
    return mtask->hasPeer(mname);
}

CORBA::Boolean Orocos_ControlTask_i::removePeer (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    std::string mname(name);
    mtask->removePeer( mname );
    return true;
}

CORBA::Boolean Orocos_ControlTask_i::connectPeers (
    ::Orocos::ControlTask_ptr p
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    TaskContext* t = ControlTaskProxy::Create( p );
    return mtask->connectPeers( t );
}

CORBA::Boolean Orocos_ControlTask_i::disconnectPeers (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    std::string pname(name);
    mtask->disconnectPeers( pname );
    return true;
}



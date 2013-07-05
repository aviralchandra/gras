// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/callable.hpp>
#include <boost/shared_ptr.hpp>
#include <stdexcept>
#include <map>

using namespace gras;

typedef std::map<std::string, boost::shared_ptr<CallableRegistryEntry> > CallableRegistry;

Callable::Callable(void)
{
    _call_registry = new CallableRegistry();
}

Callable::~Callable(void)
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry);
    delete cr;
}

void Callable::_register_call(const std::string &key, void *entry)
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry);
    (*cr)[key].reset(reinterpret_cast<CallableRegistryEntry *>(entry));
}

PMCC Callable::_handle_call(const std::string &key, const std::vector<PMCC> &args)
{
    CallableRegistry *cr = reinterpret_cast<CallableRegistry *>(_call_registry);
    boost::shared_ptr<CallableRegistryEntry> entry = (*cr)[key];
    if (not entry) throw std::invalid_argument("Callable: no method registered for key: " + key);
    return entry->call(args);
}

CallableRegistryEntry::CallableRegistryEntry(void)
{
    //NOP
}

CallableRegistryEntry::~CallableRegistryEntry(void)
{
    //NOP
}

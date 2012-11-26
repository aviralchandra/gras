// Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

#include <gras/sbuffer.hpp>
#include "alloc_on_node.hpp"
#include <boost/bind.hpp>

using namespace gras;

void gras::sbuffer_handle_deref(SBufferImpl *impl)
{
    //call the deleter if possible
    boost::shared_ptr<SBufferDeleter> token_deleter = impl->config.token.lock();
    if (token_deleter)
    {
        SBuffer buff;
        buff.reset(impl);
        (*token_deleter)(buff);
    }
    else if (impl->config.deleter)
    {
        SBuffer buff;
        buff.reset(impl);
        impl->config.deleter(buff);
        impl->config.deleter = SBufferDeleter(); //reset deleter, so we dont double delete
    }
    else
    {
        delete impl; //its really dead now
    }
}

SBufferConfig::SBufferConfig(void)
{
    memory = NULL;
    length = 0;
    affinity = -1;
}

static void numa_mem_deleter(SBuffer &buff)
{
    FreeOnNode(buff.get_actual_memory(), buff.get_actual_length());
}

static void default_allocator_deleter(SBuffer &, char *m)
{
    delete [] m;
}

static void default_allocator(SBufferConfig &config)
{
    if (config.affinity == -1)
    {
        char *m = new char[config.length + GRAS_MAX_ALIGNMENT - 1];
        size_t x = size_t(m) + GRAS_MAX_ALIGNMENT - 1;
        x -= x % GRAS_MAX_ALIGNMENT;
        config.memory = (void *)x;
        config.deleter = boost::bind(&default_allocator_deleter, _1, m);
    }
    else
    {
        config.memory = AllocOnNode(config.affinity, config.length);
        config.deleter = boost::bind(&numa_mem_deleter, _1);
        //deal with numa failue case //TODO print warning message
        if (config.memory == NULL)
        {
            config.affinity = -1;
            default_allocator(config);
        }
    }
}

SBuffer::SBuffer(const SBufferConfig &config):
    offset(0),
    length(0)
{
    this->reset(new SBufferImpl(config));
    if (config.memory == NULL)
    {
        default_allocator((*this)->config);
    }
    this->length = this->get_actual_length();
}

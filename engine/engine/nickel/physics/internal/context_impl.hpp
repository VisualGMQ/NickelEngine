#pragma once
#include "nickel/common/assert.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "NvBlastTk.h"

namespace nickel::physics {


class PhysXErrorCallback: public physx::PxErrorCallback
{
public:
    virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
    {
        constexpr const char* log_tmpl = "[PhysX][{}]({} {}ln): {}";
        switch (code) {
            case physx::PxErrorCode::eNO_ERROR:
                return;
            case physx::PxErrorCode::eDEBUG_INFO:
                LOGD(log_tmpl, "", file, line, message);
                break;
            case physx::PxErrorCode::eDEBUG_WARNING:
                LOGW(log_tmpl, "", file, line, message);
                break;
            case physx::PxErrorCode::eINVALID_PARAMETER:
                LOGW(log_tmpl, "Invalid Parameter", file, line, message);
                break;
            case physx::PxErrorCode::eINVALID_OPERATION:
                LOGW(log_tmpl, "Invalid Operation", file, line, message);
                break;
            case physx::PxErrorCode::eOUT_OF_MEMORY:
                LOGW(log_tmpl, "Out Of Memory", file, line, message);
                break;
            case physx::PxErrorCode::eINTERNAL_ERROR:
                LOGE(log_tmpl, "Internal Error", file, line, message);
                break;
            case physx::PxErrorCode::eABORT:
                LOGC(log_tmpl, "", file, line, message);
                break;
            case physx::PxErrorCode::ePERF_WARNING:
                LOGW(log_tmpl, "Performance", file, line, message);
                break;
        }

        NICKEL_CANT_REACH();
    }
};


class ContextImpl {
public:
    ContextImpl();
    ~ContextImpl();
    
private:
    physx::PxFoundation* m_foundation;
    physx::PxPhysics* m_physics; 
    PhysXErrorCallback m_error_callback;
    physx::PxDefaultAllocator m_allocator;
    Nv::Blast::TkFramework* m_blast_framework;
};

}
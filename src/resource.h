#pragma once

#include "cpp-sdk/SDK.h"
#include "Log.h"
#include "helpers/timer.h"
#include "angelscript/include/angelscript.h"
#include "angelscript/addon/scriptarray/scriptarray.h"
#include "angelscript/addon/scriptbuilder/scriptbuilder.h"
#include "angelscript/addon/scripthelper/scripthelper.h"

class AngelScriptRuntime;
class AngelScriptResource : public alt::IResource::Impl
{  
    AngelScriptRuntime* runtime;
    alt::IResource* resource;
    asIScriptModule* module = nullptr;
    asIScriptContext* context = nullptr;

    // Timers
    std::unordered_map<uint32_t, Helpers::Timer*> timers;
    std::vector<uint32_t> invalidTimers;
    uint32_t nextTimerId = 1;

    // first = event type, second = script callback
    std::vector<std::pair<alt::CEvent::Type, asIScriptFunction*>> eventHandlers;
    std::unordered_multimap<std::string, asIScriptFunction*> customLocalEventHandlers;
    std::unordered_multimap<std::string, asIScriptFunction*> customRemoteEventHandlers;

public:
    AngelScriptResource(AngelScriptRuntime* runtime, alt::IResource* resource) : runtime(runtime), resource(resource) {};
    ~AngelScriptResource() = default;

    alt::IResource* GetResource()
    {
        return resource;
    }
    AngelScriptRuntime* GetRuntime()
    {
        return runtime;
    }
    asIScriptContext* GetContext()
    {
        return context;
    }
    asIScriptModule* GetModule()
    {
        return module;
    }

    // Returns the main function if found, otherwise nullptr
    asIScriptFunction* RegisterMetadata(CScriptBuilder& builder);

    alt::String ReadFile(alt::String path);

    // Registers a new script callback for the specified event
    void RegisterEventHandler(alt::CEvent::Type event, asIScriptFunction* handler)
    {
        eventHandlers.push_back({event, handler});
    }
    // Gets all script event handlers of the specified type
    std::vector<asIScriptFunction*> GetEventHandlers(alt::CEvent::Type event)
    {
        std::vector<asIScriptFunction*> events;
        for(auto handler : eventHandlers)
        {
            if(handler.first == event) events.push_back(handler.second);
        }
        return events;
    }
    
    void RegisterCustomEventHandler(const std::string& name, asIScriptFunction* handler, bool local = true)
    {
        if(local) customLocalEventHandlers.insert({name, handler});
        else customRemoteEventHandlers.insert({name, handler});
    }
    std::vector<asIScriptFunction*> GetCustomEventHandlers(const std::string& name, bool local = true)
    {
        std::vector<asIScriptFunction*> arr;
        if(local) 
        {
            auto range = customLocalEventHandlers.equal_range(name);
            for (auto it = range.first; it != range.second; it++) arr.push_back(it->second);
        }
        else
        {
            auto range = customRemoteEventHandlers.equal_range(name);
            for (auto it = range.first; it != range.second; it++) arr.push_back(it->second);
        }
        return arr;
    }
    void HandleCustomEvent(const alt::CEvent* event, bool local = true);

    // Creates a new timer
    uint32_t CreateTimer(uint32_t timeout, asIScriptFunction* callback, bool once)
    {
        uint32_t id = nextTimerId++;
        timers[id] = new Helpers::Timer{this, callback, timeout, GetTime(), once};

        return id;
    }
    void RemoveTimer(uint32_t id)
    {
        invalidTimers.emplace_back(id);
    }

    // Yoinked from v8 helpers
    int64_t GetTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}

    bool Start();
    bool Stop();

    bool OnEvent(const alt::CEvent* event);
    void OnTick();

    void OnCreateBaseObject(alt::IBaseObject* object) {}
    void OnRemoveBaseObject(alt::IBaseObject* object) {}
};


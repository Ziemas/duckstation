#include "common/assert.h"
#include "common/log.h"
#include "core/system.h"
#include "frontend-common/sdl_initializer.h"
#include "sdl_host_interface.h"
#include <SDL.h>
#include <cstdio>

static int Run(int argc, char* argv[])
{
  // parameters
  std::optional<s32> state_index;
  bool state_is_global = false;
  const char* boot_filename = nullptr;
  for (int i = 1; i < argc; i++)
  {
#define CHECK_ARG(str) !std::strcmp(argv[i], str)
#define CHECK_ARG_PARAM(str) (!std::strcmp(argv[i], str) && ((i + 1) < argc))

    if (CHECK_ARG_PARAM("-state") || CHECK_ARG_PARAM("-gstate"))
    {
      state_is_global = argv[i][1] == 'g';
      state_index = std::atoi(argv[++i]);
    }
    else if (CHECK_ARG_PARAM("-resume"))
    {
      state_index = -1;
    }
    else
    {
      boot_filename = argv[i];
    }

#undef CHECK_ARG
#undef CHECK_ARG_PARAM
  }

  // create display and host interface
  std::unique_ptr<SDLHostInterface> host_interface = SDLHostInterface::Create();
  if (!host_interface)
  {
    Panic("Failed to create host interface");
    SDL_Quit();
    return -1;
  }

  // boot/load state
  if (boot_filename)
  {
    SystemBootParameters boot_params;
    boot_params.filename = boot_filename;
    if (host_interface->BootSystem(boot_params) && state_index.has_value())
      host_interface->LoadState(state_is_global, state_index.value());
  }
  else if (state_index.has_value())
  {
    host_interface->LoadState(true, state_index.value());
  }

  // run
  host_interface->Run();

  // done
  host_interface.reset();
  SDL_Quit();
  return 0;
}

// SDL requires the entry point declared without c++ decoration
#undef main
int main(int argc, char* argv[])
{
  // set log flags
#ifndef _DEBUG
  const LOGLEVEL level = LOGLEVEL_INFO;
  // const LOGLEVEL level = LOGLEVEL_DEV;
  // const LOGLEVEL level = LOGLEVEL_PROFILE;
  Log::SetConsoleOutputParams(true, nullptr, level);
  Log::SetFilterLevel(level);
#else
  Log::SetConsoleOutputParams(true, nullptr, LOGLEVEL_DEBUG);
  Log::SetConsoleOutputParams(true, "Pad DigitalController MemoryCard SPU", LOGLEVEL_DEBUG);
  // Log::SetConsoleOutputParams(true, "GPU GPU_HW_OpenGL SPU Pad DigitalController", LOGLEVEL_DEBUG);
  // Log::SetConsoleOutputParams(true, "GPU GPU_HW_OpenGL Pad DigitalController MemoryCard InterruptController SPU
  // MDEC", LOGLEVEL_DEBUG); g_pLog->SetFilterLevel(LOGLEVEL_TRACE);
  Log::SetFilterLevel(LOGLEVEL_DEBUG);
  // Log::SetFilterLevel(LOGLEVEL_DEV);
#endif

  FrontendCommon::EnsureSDLInitialized();

  // return NoGUITest();
  return Run(argc, argv);
}

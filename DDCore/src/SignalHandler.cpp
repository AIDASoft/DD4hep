//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================

// Framework include files
#include <DD4hep/Printout.h>
#include <DD4hep/SignalHandler.h>

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unistd.h>
#include <execinfo.h>

using namespace dd4hep;

using signal_handler_t = SignalHandler::signal_handler_t;

namespace {
  static bool s_exit_handler_print  = true;
  static bool s_exit_handler_active = false;
  static bool s_exit_handler_backtrace = false;
  static bool s_exit_handler_sleep_on_fatal = false;

  template<class T> union func_cast   {
    void* ptr;
    T     fun;
    explicit func_cast(T t) { fun = t; }
    explicit func_cast(void* t) { ptr = t; }
  };
}

/**@class SignalHandler::implementation
 *
 * Small class to manipulate default signal handling
 *
 * \author M.Frank
 */
class SignalHandler::implementation {
protected:
  struct sig_entry_t  {
    void* user_context { nullptr };
    signal_handler_t user_handler { nullptr };
  };
  struct sig_handler_t  {
    std::string name { };
    struct sigaction old_action { };
    struct sigaction handler_action { };
    std::vector<sig_entry_t> user_handlers { };
  };

public:
  typedef std::map<int, sig_handler_t> SigMap;
  SigMap  m_map;

public:
  /// Default constructor
  implementation();
  /// Default destructor
  ~implementation();
  /// Singleton accessor
  static implementation& instance();
  /// Initialize the exit handler. Subscribe to default signals
  void init();
  /// Install handler for a single signal
  void install(int num, const std::string& name, struct sigaction& action);
  /// Subscribe to a given signal with a user context and a user handler. The context MUST be unique!
  int subscribe(int signum, void* user_context, signal_handler_t handler);
  /// Unsubscribe from a given signal with a user context identifier
  int unsubscribe(int signum, void* user_context);
  /// Create simple backtrace
  void back_trace(int /* signum */);
  /// Static handler callback for system signal handler
  static void handler(int signum, siginfo_t *info,void * );
};

/// Default constructor
SignalHandler::implementation::implementation()  {
}

/// Default destructor
SignalHandler::implementation::~implementation()  {
}

/// Singleton accessor
SignalHandler::implementation& SignalHandler::implementation::instance()  {
  static std::unique_ptr<implementation> imp;
  if ( !imp )  {
    imp = std::make_unique<implementation>();
  }
  return *imp;
}

/// Initialize the exit handler. Subscribe to default signals
void SignalHandler::implementation::init()  {
  struct sigaction new_action;
  sigemptyset(&new_action.sa_mask);
  new_action.sa_handler   = 0;
  new_action.sa_sigaction = handler;
  new_action.sa_flags     = SA_SIGINFO;

  install(SIGILL,  "SIGILL",  new_action);
  install(SIGINT,  "SIGINT",  new_action);
  install(SIGTERM, "SIGTERM", new_action);
  install(SIGHUP,  "SIGHUP",  new_action);

  install(SIGQUIT, "SIGQUIT", new_action);
  install(SIGBUS,  "SIGBUS",  new_action);
  install(SIGXCPU, "SIGXCPU", new_action);
  sigaddset(&new_action.sa_mask,SIGSEGV);
  sigaddset(&new_action.sa_mask,SIGABRT);
  sigaddset(&new_action.sa_mask,SIGFPE);
  install(SIGABRT, "SIGABRT", new_action);
  install(SIGFPE,  "SIGFPE",  new_action);
  install(SIGSEGV, "SIGSEGV", new_action);
}

/// Subscribe to a given signal with a user context and a user handler. The context MUST be unique!
int SignalHandler::implementation::subscribe(int signum, void* user_context, signal_handler_t user_handler)   {
  if ( m_map.empty() )  {
    this->init();
  }
  auto ihandler = m_map.find(signum);
  if ( ihandler == m_map.end() )   {
    char text[32];
    struct sigaction new_action;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_handler   = 0;
    new_action.sa_sigaction = SignalHandler::implementation::handler;
    new_action.sa_flags     = SA_SIGINFO;
    ::snprintf(text, sizeof(text),"%08X",signum);
    install(signum, text, new_action);
    ihandler = m_map.find(signum);
  }
  if ( ihandler != m_map.end() )   { // Should always be true
    sig_entry_t entry {user_context, user_handler};
    ihandler->second.user_handlers.emplace_back(entry);
    return 1;
  }
  return 0;
}

/// Unsubscribe from a given signal with a user context identifier
int SignalHandler::implementation::unsubscribe(int signum, void* user_context)   {
  auto ihandler = m_map.find(signum);
  if ( ihandler != m_map.end() )   {
    auto & handlers = ihandler->second.user_handlers;
    for( auto it = handlers.begin(); it != handlers.end(); ++it )   {
      if ( it->user_context == user_context )   {
        handlers.erase(it);
        return 1;
      }
    }
  }
  return 0;
}

/// Create simple backtrace
void SignalHandler::implementation::back_trace(int /* signum */) {
  if ( s_exit_handler_backtrace )   {
    void *bt[256];
    char text[512];
    int bt_size = ::backtrace(bt, sizeof(bt) / sizeof(void *));
    size_t len = ::snprintf(text, sizeof(text), "\n[INFO] (ExitSignalHandler) %s\n",
                            "---------------------- Backtrace ----------------------\n");
    text[sizeof(text)-2] = '\n';
    text[sizeof(text)-1] = 0;
    ::write(STDERR_FILENO, text, len);
    len = ::snprintf(text, sizeof(text), "[INFO] Number of elements in backtrace: %d\n", bt_size);
    text[sizeof(text)-2] = '\n';
    text[sizeof(text)-1] = 0;
    ::write(STDERR_FILENO, text, len);
    ::backtrace_symbols_fd(bt, bt_size, STDERR_FILENO);
    for (int i = 0; i < bt_size; i++) {
      len = ::snprintf(text,sizeof(text),"[INFO] (SignalHandler) %02d --> %p\n", i, bt[i]);
      text[sizeof(text)-2] = '\n';
      text[sizeof(text)-1] = 0;
      ::write(STDERR_FILENO, text, len);
    }
  }
}

/// Install handler for a single signal
void SignalHandler::implementation::install(int num, const std::string& name, struct sigaction& action) {
  auto& action_entry = m_map[num];
  int res = ::sigaction (num, &action, &action_entry.old_action);
  if ( res != 0 ) {
    char text[512];
    auto len = ::snprintf(text,sizeof(text),"Failed to install exit handler for %s", name.c_str());
    text[sizeof(text)-2] = '\n';
    text[sizeof(text)-1] = 0;
    ::write(STDERR_FILENO, text, len);
    return;
  }
  action_entry.handler_action = action;
  action_entry.name = name;
}
  
/// Static handler callback for system signal handler
void SignalHandler::implementation::handler(int signum, siginfo_t *info, void *ptr) {
  SigMap& m = instance().m_map;
  SigMap::iterator iter_handler = m.find(signum);
  s_exit_handler_active = true;
  if ( iter_handler != m.end() ) {
    auto hdlr = iter_handler->second.old_action.sa_handler;
    func_cast<void (*)(int)> dsc0(hdlr);
    func_cast<void (*)(int,siginfo_t*, void*)> dsc(dsc0.ptr);

    if ( s_exit_handler_print ) {{
        char text[512];
        size_t len = ::snprintf(text,sizeof(text),
                                "[FATAL] (SignalHandler) Handle signal: %d [%s] Old action:%p Mem:%p Code:%08X\n",
                                signum,iter_handler->second.name.c_str(),dsc.ptr,info->si_addr,info->si_code);
        text[sizeof(text)-2] = '\n';
        text[sizeof(text)-1] = 0;
        ::write(STDERR_FILENO,text,len);
        // Debugging hack, if enabled (default: NO)
        if ( s_exit_handler_sleep_on_fatal )  {
          bool _s_sleep = true;
          len = ::snprintf(text,sizeof(text),
                           "[FATAL] (SignalHandler) Sleeping for debugging.... %s\n",
                           _s_sleep ? "YES" : "NO");
          text[sizeof(text)-2] = '\n';
          text[sizeof(text)-1] = 0;
          ::write(STDERR_FILENO,text,len);
          while ( _s_sleep ) ::usleep(100000);
        }
      }
      if ( !iter_handler->second.user_handlers.empty() )    {
        auto& handlers = iter_handler->second.user_handlers;
        for( auto ih = handlers.rbegin(); ih != handlers.rend(); ++ih )   {
          if ( ih->user_handler )  {
            bool ret = (*(ih->user_handler))(ih->user_context, signum);
            if ( ret )   {
              return;
            }
            // Otherwise continue signal processing and eventually call default handlers
          }
          // No handler fired: call previously registered signal handler
          auto& entry = iter_handler->second.old_action;
          if ( entry.sa_handler )
            (*entry.sa_handler)(signum);
          else if ( entry.sa_sigaction )
            (*entry.sa_sigaction)(signum, info, ptr);
        }
      }
      if ( signum == SIGSEGV || signum == SIGBUS || signum == SIGILL || signum == SIGABRT )  {
        instance().back_trace(signum);
      }
      else if ( info->si_signo == SIGSEGV || info->si_signo == SIGBUS || info->si_signo == SIGILL || info->si_signo == SIGABRT )  {
        instance().back_trace(info->si_signo);
      }
    }
    if ( signum == SIGINT || signum == SIGHUP || signum == SIGFPE || signum == SIGPIPE ) {
      if ( dsc.fun && (dsc0.fun != SIG_IGN) )
        dsc.fun(signum, info, ptr);
      else if ( signum == SIGHUP )
        ::_exit(128+signum);
    }
    else if ( signum == SIGSEGV && hdlr && hdlr != SIG_IGN && hdlr != SIG_DFL ) {
      ::_exit(128+signum);
    }
    else if ( hdlr && hdlr != SIG_IGN && dsc.fun )  {
      dsc.fun(signum, info, ptr);
    }
    else if ( hdlr == SIG_DFL ) {
      ::_exit(128+signum);
    }
  }
  s_exit_handler_active = false;
}

/// Default constructor
SignalHandler::SignalHandler()
{
}

/// Default destructor
SignalHandler::~SignalHandler()  {
}

/// (Re-)apply registered interrupt handlers to override potentially later registrations by other libraries
void SignalHandler::applyHandlers()  {
  auto& imp = implementation::instance();
  struct sigaction old_action { };
  printout(INFO, "SignalHandler", "++ Re-apply signal handlers");
  for( const auto& e : imp.m_map )  {
    ::sigaction (e.first, &e.second.handler_action, &old_action);
    printout(DEBUG, "SignalHandler",
             "++ Re-apply signal handler for %-10s [%3ld entries]",
             e.second.name.c_str(), e.second.user_handlers.size());
  }
}

/// Install handler for any signal
bool SignalHandler::registerHandler(int sig_num, void* param, signal_handler_t handler)  {
  return implementation::instance().subscribe(sig_num, param, handler) == 1;
}

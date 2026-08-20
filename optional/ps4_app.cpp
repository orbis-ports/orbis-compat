#include "ps4_app.h"

#include <orbis_log.h>

// Defined below, beside the rest of the logging; declared here because ps4_app_init registers it.
static void ps4_vlog(const char* fmt, va_list ap);
static void ps4_vlog_fatal(const char* fmt, va_list ap);

#include <orbis/libkernel.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace {

const char* s_app = "ps4";

// ---------------------------------------------------------------- the run config
//
// See ps4_app.h. One optional file, read once, no runtime detection of anything.
// The path is /app0 because that is the only directory a title can count on: on the
// console it is the installed package root, and unemups4 union-mounts the loaded
// executable's own directory there (app/unemups4/src/main.rs, "Also union-mount the
// loaded title's own directory onto /app0"), so the harness only has to drop the
// file next to the ELF it is about to launch.
const char* const RunCfgPath = "/app0/ps4-run.cfg";

int s_autoexit  = 0;
int s_frameKlog = 0;
// What the read actually found, so ps4_app_init can say it on both channels once
// netlog is up. A capture-sized buffer would be pointless: this is a handful of
// key=value lines.
char s_runCfgNote[128] = "run-cfg: not read yet";

// Trailing spaces, CR and any inline comment removed; the result is compared whole.
void trim(char* s) {
  char* hash = s;
  while(*hash!='\0' && *hash!='#')
    ++hash;
  *hash = '\0';
  size_t n = strlen(s);
  while(n>0 && (s[n-1]==' ' || s[n-1]=='\t' || s[n-1]=='\r'))
    --n;
  s[n] = '\0';
  }

// key=value against one line, tolerant of leading blanks and of a value that is
// "1"/"0". Anything else leaves the option alone - a config this file cannot read is
// not a reason to change what the title does.
void applyOption(char* line) {
  while(*line==' ' || *line=='\t')
    ++line;
  trim(line);
  if(line[0]=='\0')
    return;
  char* eq = strchr(line,'=');
  if(eq==nullptr)
    return;
  *eq = '\0';
  const char* key = line;
  const char* val = eq+1;
  const int   on  = (strcmp(val,"1")==0);
  if(strcmp(key,"autoexit")==0)
    s_autoexit = on;
  else if(strcmp(key,"frame-klog")==0)
    s_frameKlog = on;
  }

// O_RDONLY. <fcntl.h> is not part of the orbis/ headers this file already includes
// and the value is fixed by the ABI, so it is spelled out rather than dragged in.
const int OpenReadOnly = 0;

void readRunConfig() {
  const int32_t fd = sceKernelOpen(RunCfgPath,OpenReadOnly,0);
  if(fd<0) {
    // The ordinary case on a console: a .pkg contains eboot.bin, param.sfo, the two
    // PRXs and whatever EXTRA_FILES asked for, and nothing puts this file there.
    snprintf(s_runCfgNote,sizeof(s_runCfgNote),
             "run-cfg: no %s (0x%08x) - console defaults, autoexit=0 frame-klog=0",
             RunCfgPath,int(fd));
    return;
    }

  char         buf[512] = {};
  const size_t got      = sceKernelRead(fd,buf,sizeof(buf)-1);
  sceKernelClose(fd);
  // sceKernelRead returns a size_t; a negative errno arrives as a huge value, and a
  // huge value is not a config file either way.
  const size_t len = (got<sizeof(buf)) ? got : 0u;
  buf[len] = '\0';

  char* p = buf;
  while(*p!='\0') {
    char* nl = strchr(p,'\n');
    if(nl!=nullptr)
      *nl = '\0';
    applyOption(p);
    if(nl==nullptr)
      break;
    p = nl+1;
    }

  snprintf(s_runCfgNote,sizeof(s_runCfgNote),
           "run-cfg: %s read (%zu B) - autoexit=%d frame-klog=%d",
           RunCfgPath,len,s_autoexit,s_frameKlog);
  }

// orbis_netlog() is printf-style, so a line that is already formatted has to go
// through "%s" rather than be handed over as the format string.
void netLine(const char* line) {
  orbis_netlog("%s",line);
  }

// ---------------------------------------------------------------- klog is a FALLBACK now
//
// MEASURED on the console, fourteen 512-line samples across a 137 s run: `sceKernelDebugOutText`
// costs 8.3-14.9 ms PER LINE and `sceNetSendto` costs 13-22 microseconds. Three orders of magnitude.
// The independent control is the log's own timestamps - 5591 inter-line gaps, median 8.0 ms, summing
// to 50.2 s of the 136.8 s run: 37% of a run was inside that one syscall.
//
// So the two channels are no longer both written. The one question that decides it is whether
// ANYTHING ELSE is carrying the line:
//
//   * netlog is down - nothing else is. klog is the only evidence the title is alive and it is worth
//     any price, which is the reason it was here first (see ps4_app_init below).
//   * the host asked for it - the EMULATOR leg, and this is the whole risk of the change: nothing
//     drains the UDP socket under emulation, so klog is the only channel run-tests.sh reads there.
//     `frame-klog=1` in /app0/ps4-run.cfg already says exactly this, is already written by the
//     harness on that leg only, and is already absent on every console launch.
//
// ONE FUNCTION AND NOT A PER-CALLER FLAG, deliberately: conventions.md's rule is that a knob every
// deliberate target has to set is a knob some target will forget, and the one that forgets is the one
// that breaks. Nothing outside this file decides this.
bool klogWanted() {
  return s_frameKlog || orbis_netlog_ready()==0;
  }

void formatLine(char* buf, size_t sz, const char* fmt, va_list ap) {
  const int n = vsnprintf(buf,sz-2,fmt,ap);
  size_t len = (n<0) ? 0u : size_t(n);
  if(len>sz-2)
    len = sz-2;
  if(len==0 || buf[len-1]!='\n') {
    buf[len] = '\n';
    ++len;
    }
  buf[len] = '\0';
  }

void tagLine(char* buf, size_t sz, const char* fmt, va_list ap) {
  const int head = snprintf(buf,sz,"[%s] ",s_app);
  const size_t off = (head<0 || size_t(head)>=sz-2) ? 0u : size_t(head);
  formatLine(buf+off,sz-off,fmt,ap);
  }

}

int ps4_run_autoexit(void) {
  return s_autoexit;
  }

int ps4_run_frame_klog(void) {
  return s_frameKlog;
  }

void ps4_app_init(const char* app, const char* stamp) {
  // Before anything else can have something to report: orbis-compat's corrections are no-ops until
  // a logger is registered, and they run beneath everything here.
  orbis_set_log(ps4_vlog);

  // The second channel and the policy that follows it. orbis-compat carries the crash handlers now
  // (it is what supplies backtrace(3)), but it deliberately does not decide what a dying process
  // should do - on this console that choice is visible to the user, because returning from main()
  // is reported as CE-34878-0 and reads exactly like a crash.
  orbis_set_log_fatal(ps4_vlog_fatal);
  orbis_set_fatal_action(ps4_idle_forever);

  s_app = app;

  char buf[256] = {};
  snprintf(buf,sizeof(buf),"[%s] alive - main() entered, built %s\n",app,stamp);

  // klog first and unconditionally: if sceNet never comes up this is the only
  // evidence that the title reached main() at all.
  sceKernelDebugOutText(0,buf);

  // Before netlog, because frame-klog decides which channel the per-frame lines take
  // and a title that read its config late would have logged the first frames the
  // other way. Both channels then get to say what was found - the answer is the only
  // thing that now differs between the two legs of the harness, so it is not allowed
  // to be implicit.
  readRunConfig();

  orbis_netlog_init();
  netLine(buf);
  ps4_log("%s",s_runCfgNote);
  }

void ps4_app_set_tag(const char* app) {
  if(app!=nullptr)
    s_app = app;
  }

// ⚠ SPLIT SO orbis-compat CAN BORROW IT. The overlay's interposers - stat(), operator new, the mmap
// translation - used to call ps4_log directly, which tied files that are corrections to THIS SDK to
// an engine header, and made them impossible to move into the overlay where they belong. The overlay
// declares a hook instead (orbis_log.h) and this is what gets registered into it, in ps4_app_init.
static void ps4_vlog(const char* fmt, va_list ap) {
  char buf[512] = {};
  tagLine(buf,sizeof(buf),fmt,ap);

  // UDP first: sendto cannot block, so the line is already on the wire before
  // the klog write gets a chance to stall on an undrained debug buffer. And it is now
  // usually the ONLY write - see klogWanted(). These are 279 of the 5667 lines an OpenGothic run
  // emits, so this call site is 5% of the saving and the same one-line policy; leaving it unguarded
  // is how the expensive path stayed hidden for three sessions in the first place.
  netLine(buf);
  if(klogWanted())
    sceKernelDebugOutText(0,buf);
  }

void ps4_log(const char* fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  ps4_vlog(fmt,ap);
  va_end(ap);
  }

void ps4_log_frame(const char* fmt, ...) {
  char    buf[512] = {};
  va_list ap;
  va_start(ap,fmt);
  tagLine(buf,sizeof(buf),fmt,ap);
  va_end(ap);

  netLine(buf);
  if(klogWanted()) {
    // The host asked for it, or nothing else is carrying the line: nothing drains the UDP socket on
    // an emulator run, so klog is the only channel the harness reads there, and an emulated klog has
    // none of the back-pressure that bans this call from a console render loop. `s_frameKlog` used to
    // be read here directly; klogWanted() adds the netlog-is-down arm, which this call site wanted
    // all along - a per-frame line on a console with no netlog went nowhere at all.
    sceKernelDebugOutText(0,buf);
    }
  }

// A PRE-FORMATTED LINE, tag and newline already applied by the caller, and it owns the klog decision
// for those lines too - so the policy lives in the one file that read the run config, not at each
// call site. The name says `net` and the behaviour is wider; it is kept because callers name it.
//
// ⚠ ITS ORIGINAL CONSUMER IS GONE. This was written for the GNM backend's log sink, which produced
// 5388 of the 5667 lines an OpenGothic run emitted - 95% of the log, and 95% of the 37% of wall time
// klog was costing. That backend is not on this branch. Nothing calls this today; it is kept because
// any backend with its own log stream wants exactly this shape.
void ps4_log_net_line(const char* line) {
  netLine(line);
  if(klogWanted())
    sceKernelDebugOutText(0,line);
  }

// klog FIRST and unconditionally - see ps4_app.h for why the order is the whole point.
//
// Split into a va_list form so orbis-compat can be handed the same channel: its crash handlers call
// orbis_log_fatal, and this is what that has to reach on this console.
static void ps4_vlog_fatal(const char* fmt, va_list ap) {
  char buf[512] = {};
  tagLine(buf,sizeof(buf),fmt,ap);

  sceKernelDebugOutText(0,buf);
  netLine(buf);
  }

void ps4_log_fatal(const char* fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  ps4_vlog_fatal(fmt,ap);
  va_end(ap);
  }

void ps4_idle_forever(const char* what) {
  if(s_autoexit) {
    ps4_log("%s - returning from main (run-cfg autoexit=1)",what);
    return;
    }
  // The default, and the only behaviour a .pkg launch can reach: returning from
  // main() is what the console reports as CE-34878-0, which reads exactly like a
  // crash.
  ps4_log("%s - idle, close the title with the PS button",what);
  for(unsigned sec=5; ; sec+=5) {
    sceKernelUsleep(5*1000*1000);
    ps4_log_frame("idle %us",sec);
    }
  }

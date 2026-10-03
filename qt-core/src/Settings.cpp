#include "pomo/Settings.h"
namespace pomo { Settings& Settings::instance() { static Settings s; return s; } }

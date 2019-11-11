// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

#include "Common.h"
#include "Exceptions.h"
#include "Log.h"

#if defined(_WIN32)
#include <windows.h>
#endif

#include <aleth/buildinfo.h>

using namespace std;

namespace dev
{
char const* Version = aleth_get_buildinfo()->project_version;
bytes const NullBytes;
std::string const EmptyString;

void InvariantChecker::checkInvariants(HasInvariants const* _this, char const* _fn, char const* _file, int _line, bool _pre)
{
    if (!_this->invariants())
    {
        cwarn << (_pre ? "Pre" : "Post") << "invariant failed in" << _fn << "at" << _file << ":" << _line;
        ::boost::exception_detail::throw_exception_(FailedInvariant(), _fn, _file, _line);
    }
}

TimerHelper::~TimerHelper()
{
    auto e = std::chrono::high_resolution_clock::now() - m_t;
    if (!m_ms || e > chrono::milliseconds(m_ms))
        clog(VerbosityDebug, "timer")
            << m_id << " " << chrono::duration_cast<chrono::milliseconds>(e).count() << " ms";
}

int64_t utcTime()
{
    // TODO: Fix if possible to not use time(0) and merge only after testing in all platforms
    // time_t t = time(0);
    // return mktime(gmtime(&t));
    return time(0);
}

string inUnits(bigint const& _b, strings const& _units)
{
    ostringstream ret;
    u256 b;
    if (_b < 0)
    {
        ret << "-";
        b = (u256)-_b;
    }
    else
        b = (u256)_b;

    u256 biggest = 1;
    for (unsigned i = _units.size() - 1; !!i; --i)
        biggest *= 1000;

    if (b > biggest * 1000)
    {
        ret << (b / biggest) << " " << _units.back();
        return ret.str();
    }
    ret << setprecision(3);

    u256 unit = biggest;
    for (auto it = _units.rbegin(); it != _units.rend(); ++it)
    {
        auto i = *it;
        if (i != _units.front() && b >= unit)
        {
            ret << (double(b / (unit / 1000)) / 1000.0) << " " << i;
            return ret.str();
        }
        else
            unit /= 1000;
    }
    ret << b << " " << _units.front();
    return ret.str();
}

/*
The equivalent of setlocale(LC_ALL, “C”) is called before any user code is run.
If the user has an invalid environment setting then it is possible for the call
to set locale to fail, so there are only two possible actions, the first is to
throw a runtime exception and cause the program to quit (default behaviour),
or the second is to modify the environment to something sensible (least
surprising behaviour).

The follow code produces the least surprising behaviour. It will use the user
specified default locale if it is valid, and if not then it will modify the
environment the process is running in to use a sensible default. This also means
that users do not need to install language packs for their OS.
*/
void setDefaultOrCLocale()
{
#if __unix__
    if (!setlocale(LC_ALL, ""))
        setenv("LC_ALL", "C", 1);
#endif

#if defined(_WIN32)
    // Change the code page from the default OEM code page (437) so that UTF-8 characters are
    // displayed correctly in the console.
    SetConsoleOutputCP(CP_UTF8);
#endif
}

bool ExitHandler::s_shouldExit = false;

bool isTrue(std::string const& _m)
{
    return _m == "on" || _m == "yes" || _m == "true" || _m == "1";
}

bool isFalse(std::string const& _m)
{
    return _m == "off" || _m == "no" || _m == "false" || _m == "0";
}

string Demangle(const char* name)
{  
    string dname;
    int status = 0;
    char* pdname = abi::__cxa_demangle(name, NULL, 0, &status);     
    if(status == 0)
    {
        dname.assign(pdname);
        free(pdname);
    }
    else
    {
        dname.assign("[unknown function]");
    }
    return dname;
}

void DumpStack(void)
{
	#define SIZE 10000
	
	int nptrs;
	void *buffer[SIZE];
	char **strings;
	nptrs = backtrace(buffer, SIZE);
 
	strings = backtrace_symbols(buffer, nptrs);
	if (strings == NULL) {
		cdebug << "backtrace_symbols";
		exit(EXIT_FAILURE);
	}

	for(int i = 1; i < nptrs; ++i)
	{
		const string funinfo(strings[i]);
		const int posleftparenthesis = funinfo.rfind('(');
		const int posplus = funinfo.rfind('+');
		const int posleftbracket = funinfo.rfind('[');
		const int posrightbracket = funinfo.rfind(']');

		long long unsigned int offset = strtoull(funinfo.substr(posleftbracket+1, posrightbracket-posleftbracket-1).c_str(), NULL, 0);
		const string module_name = funinfo.substr(0, posleftparenthesis);
		const string function = Demangle(funinfo.substr(posleftparenthesis+1, posplus-posleftparenthesis-1).c_str());

		cdebug << offset << "#" << module_name << "#" << function;
	}
	
	free(strings);
}


}  // namespace dev

#include "Aerc.h"
#if defined (_WIN32)
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif // WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	include <TimeAPI.h>
#	include <locale>
#	include <codecvt>
#	include <vector>

std::wstring utf8toUtf16(const std::string & str)
{
	using std::runtime_error;
	using std::string;
	using std::vector;
	using std::wstring;

	if (str.empty())
		return wstring();

	size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), NULL, 0);
	if (charsNeeded == 0)
		throw runtime_error("Failed converting UTF-8 string to UTF-16");
#pragma warning( push )
#pragma warning( disable : 4267)
	vector<wchar_t> buffer(charsNeeded);
	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), &buffer[0], buffer.size());
	if (charsConverted == 0)
		throw runtime_error("Failed converting UTF-8 string to UTF-16");
#pragma warning( pop ) 
	return wstring(&buffer[0], charsConverted);
}
#else 
#	include <dirent.h>
#endif

#if defined(__linux__)
#	include <errno.h> // Perhaps not needed errno
#	include <fcntl.h>
#	include <linux/limits.h>
#	include <stdlib.h>
#	include <sys/inotify.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <unistd.h>
#endif

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4127)
#	include "yaml-cpp/yaml.h"
#	pragma warning(pop)	
#else
#	include "yaml-cpp/yaml.h"
#endif

#include "Archiver.h"
#include "Compiler.h"

bool FileExists(const char* _path)
{
//	assert(_path[0]=='\0');
#if defined(_WIN32)
	DWORD attr = GetFileAttributesW(utf8toUtf16(_path).c_str());
	if (attr == 0xFFFFFFFF)
	{
		switch (GetLastError())
		{
		case ERROR_FILE_NOT_FOUND:
		case ERROR_PATH_NOT_FOUND:
		case ERROR_NOT_READY:
		case ERROR_INVALID_DRIVE:
			return false;
		default:
			return false;
		//	handleLastErrorImpl(_path);
		}
	}
	return true;
#else
	struct stat st;
	return stat(_path, &st) == 0;
#endif
}

std::string StringReplace(const std::string& _str, const std::string& _before, const std::string& _after) {
	size_t pos = 0;
	std::string retString = _str;
	while ((pos = retString.find(_before.c_str(), pos)) != std::string::npos) {
		retString.replace(pos, _before.length(), _after.c_str());
		pos += _after.length();
	}
	return retString.c_str();
}

std::string GetCurrentPath()
{
#if defined(_WIN32)
    wchar_t result_path[MAX_PATH];
	result_path[0] = 0;

    DWORD dwRet = GetCurrentDirectoryW(MAX_PATH, result_path);
    if (dwRet == 0)
    {
    	return "";
        // Handle an error condition.
        //printf("GetFullPathName failed (%d)\n", GetLastError());
    }
    wcscat(result_path, L"\\");

    /* Replace '\\' with / */
    int i = 0;
    while (result_path[i])
    {
        if (result_path[i] == L'\\')
            result_path[i] = L'/';
        i++;
    }
	std::wstring tmp = result_path;
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return _strdup(converter.to_bytes(tmp).c_str());
#else
	// could leak maybe result_path needs to be freed. ?
    char result_path[PATH_MAX];
    result_path[0] = 0;
    char* resultError = 0;
    resultError = getcwd(result_path, PATH_MAX);
    if(!resultError) {
    	return "";
    }
    return result_path;
#endif
}


bool SetCurrentPath(const std::string &_directory)
{
#if defined(_WIN32)
    if (SetCurrentDirectoryW(utf8toUtf16(_directory).c_str()) == FALSE) {
        return false;
    }
#else
    if (chdir(_directory.c_str()) != 0) {
        return false;
    }
#endif
    return true;
}

enum File_Type{
	AERC_FILE,
	AERC_SHADER
};

struct Aerc_File{
	int			type;
	std::string path;
};

struct Aerc_Bundle{
	std::string type;
	std::string output;
	std::vector<std::string> files;
	std::vector<std::string> shaders;
};

void operator >> (const YAML::Node& node, Aerc_File& file) {
   if(node["file"].IsDefined()){
	   file.type = AERC_FILE;
	   file.path = node["file"].as<std::string>();
   }else if(node["shader"].IsDefined()){
	   file.type = AERC_SHADER;
	   file.path = node["shader"].as<std::string>();	   
   } else {
	   std::cout << "Error Parsing file inside files. Remember valid types are file: or shader:" << std::endl;
   }
}

void operator >> (const YAML::Node& node, Aerc_Bundle& bundle ) {
	if(node["bundle"].IsDefined())
		bundle.type = node["bundle"].as<std::string>();
	bundle.output = node["output"].as<std::string>();
	const YAML::Node& files = node["files"];
	for(unsigned i=0;i<files.size();i++){
		Aerc_File file;
		files[i] >> file;
		if(file.type == AERC_FILE)
			bundle.files.push_back(file.path);
		else
			bundle.shaders.push_back(file.path);
	  }
}

bool ProcessBundle(Aerc_Bundle& bundle){
	Compiler c;
	c.SetFileName(bundle.output.c_str());
	const std::vector<std::string> files = bundle.files;
	for(unsigned i=0;i<files.size();i++){
		if(!c.Add(files[i].c_str())) return false;
	}

	return true;
}

bool FromFile(const char* _file){

	if(!FileExists(_file)){
		std::cout<< "Your input " << _file << " does not seem to be a file.";
		return false;	
	}
	const std::string old_cur = GetCurrentPath();
	const std::string tmp = StringReplace(_file, "\\", "/");
	const std::string folder = tmp.substr(0, tmp.find_last_of('/'));
	const std::string file = tmp.substr(tmp.find_last_of('/')+1, tmp.size());

	if(!SetCurrentPath(folder)){
		std::cout << "Unable to change current path to " << folder << std::endl;
		return false;
	}

	YAML::Node doc;

	try {
		 doc = YAML::LoadFile(file);
	} catch(...) {
		std::cout << "errors when parsing " << _file << std::endl;
		SetCurrentPath(old_cur);
		return false;
	}

	for(unsigned i=0;i<doc.size();i++) {
		Aerc_Bundle comp;
		doc[i] >> comp;
		ProcessBundle(comp);
	}

	SetCurrentPath(old_cur);
	return true;
}
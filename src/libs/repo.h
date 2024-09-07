#ifndef REPO_H
#define REPO_H
#include <filesystem>
#include <string>
#include <iostream>
#include <fstream>

#define CHUNK 16384
#define SVC_DIR ".svc"
#define APPEND(file,contents) std::ofstream( Repo::get_svc_dir() / file, std::ios::app ) << contents
#define LOG(logfile,contents) APPEND( "logs" #logfile, contents )

namespace Repo {
	std::filesystem::path get_svc_dir();
	void update_ref(const char*, std::string);
	std::string get_ref(const char*);

	extern bool created_new; // flag = true if create_obj finds hashed file
}

#endif
#include "commit.h"

namespace Commands {
	void Commit::exec() {
		std::filesystem::path vcs_dir = get_svc_dir();
		if (vcs_dir.empty()) {
			ERR(std::filesystem::current_path().string() << " is not being tracked.");
			SEE_HELP(init)
			return;
		}
		
		std::string msg = this->parser->match_arg();
		if (msg == "" || !this->parser->is_parsed()) {
			ERR("Usage: vcs commit <msg>")
			SEE_HELP(commit)
			return;
		}
	}

	void Commit::help() {
		std::cout << "commit\n";
		std::cout << "Description: Save snapshots of staged files. Requires directory to be tracked (vcs init).\n";
		std::cout << "Usage: svc commit <msg>\n";
		std::cout << "Params:\n";
		std::cout << "\tmsg: Message to associate with commit\n";
	}
}
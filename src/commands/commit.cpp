#include "commit.h"

namespace Commands {
	void Commit::exec() {
		// Get tracking directory
		std::filesystem::path svc_dir = Repo::get_svc_dir();
		if (svc_dir.empty()) {
			ERR(std::filesystem::current_path().string() << " is not being tracked.");
			SEE_HELP(init)
			return;
		}
		
		// Check for commit message
		std::string msg = this->parser->match_arg();
		if (msg == "" || !this->parser->is_parsed()) {
			ERR("Usage: vcs commit <msg>")
			SEE_HELP(commit)
			return;
		}
		
		try {
			auto staging_dict = Dict::read_dict_file("STAGING");
			auto prev_commit = Repo::get_ref("head");
			auto staging_trie = Dict::build_trie(staging_dict);

			std::vector<std::string> commit_root;
			commit_root.push_back(staging_trie->get_val());

			Objects::create_commit_obj(msg, commit_root, prev_commit.empty() ? NULL : &prev_commit);

			delete staging_trie;
		} catch (std::string err) {
			ERR(err);
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
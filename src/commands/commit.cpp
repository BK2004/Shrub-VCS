#include "commit.h"
#include "../libs/sha256.h"

namespace Commands {
	void Commit::exec() {
		// Get tracking directory
		std::filesystem::path svc_dir = get_svc_dir();
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
			// Go to staging directory and create obj for each child of staging dir
			std::filesystem::path staging_dir = svc_dir / "staging";
			int num_subfiles = 0;
			for (const auto & subfile : std::filesystem::directory_iterator(staging_dir)) {
				auto new_path = this->dfs_commit(subfile);
				num_subfiles++;
			}

			if (num_subfiles == 0) {
				std::cout << "No files in staging." << std::endl;
				return;
			}

			// Delete objects in staging dir if commit is successful
			for (const auto & subfile : std::filesystem::directory_iterator(staging_dir)) {
				std::filesystem::remove_all(subfile);
			}
		} catch (std::string err) {
			ERR(err);
			return;
		}
	}

	std::filesystem::path Commit::dfs_commit(const std::filesystem::directory_entry& entry) {
		// If directory, object needs to be created or linked with children
		if (entry.is_directory()) {
			std::vector<std::string> children;

			// Recursively handle children first and sort them by name in ascending order
			for (const auto & ch : std::filesystem::directory_iterator(entry)) {
				auto ch_path = this->dfs_commit(ch);
				children.push_back(ch_path.parent_path().filename().string() + ch_path.filename().string());
			}
			std::sort(children.begin(), children.end());

			// Concatenate filename with child names, and use that string for hash
			std::ostringstream concat;
			concat << entry.path().string();
			for (size_t i = 0; i < children.size(); i++) {
				concat << children[i];
			}

			return create_obj(entry.path(), sha256(concat.str()), &children);
		} else {
			// Not a directory, directly create object
			return create_obj(entry.path(), sha256_file(entry.path()), nullptr);
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
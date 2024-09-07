#include "commit.h"

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
			std::vector<std::string> children;
			for (const auto & subfile : std::filesystem::directory_iterator(staging_dir)) {
				auto new_path = this->dfs_commit(subfile);
				// Only track child if it is changed
				if (this->wrote_new)
					children.push_back(new_path.parent_path().filename().string() + new_path.filename().string());
			}

			if (children.size() == 0) {
				std::cout << "Nothing to commit." << std::endl;
			}

			// Delete objects in staging dir
			for (const auto & subfile : std::filesystem::directory_iterator(staging_dir)) {
				std::filesystem::remove_all(subfile);
			}

			if (children.size() > 0) {
				// Create commit object, link it to current head
				sort(children.begin(), children.end());
				auto prev_head = get_ref("head");
				create_commit_obj(msg, children, prev_head.empty() ? NULL : &prev_head);
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

			auto res = create_obj(entry.path(), sha256(concat.str()), &children);
			this->wrote_new = created_new;
			return res;
		} else {
			// Not a directory, directly create object
			auto res = create_obj(entry.path(), sha256_file(entry.path()), nullptr);
			this->wrote_new = created_new;
			return res;
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
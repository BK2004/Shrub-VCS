#include "add.h"

namespace Commands {
	void Add::exec() {
		// Get path to staging folder
		std::filesystem::path svc_dir = Repo::get_svc_dir();
		if (svc_dir.empty()) {
			ERR(std::filesystem::current_path().string() << " is not being tracked.");
			SEE_HELP(init)
			return;
		}

		// There should be at least 1 arg for location
		if (this->parser->is_parsed()) {
			ERR("Usage: svc add <loc1> <loc2> ...")
			return;
		}

		std::unordered_map<std::string, std::string> staging_dict;
		bool updated_dict = false;
		try {
			staging_dict = Dict::read_dict_file("STAGING");
		} catch (std::string e) {
			ERR(e)
			return;
		}

		auto copy_ops = std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive;
		while (!this->parser->is_parsed()) {
			std::string loc = this->parser->match_arg();

			if (!std::filesystem::exists(loc)) {
				std::cout << loc << " does not exist.\n";
				continue;
			}

			std::filesystem::path loc_path = std::filesystem::canonical(loc);
			if (svc_dir.parent_path().string().size() > loc_path.string().size()) {
				std::cout << "Cannot add files outside of directory. (" << loc << ")\n";
				return;
			}

			// Ignore SVC_DIR (and .git for now)
			if (loc_path == svc_dir || loc_path.filename() == ".git") continue;

			auto relative_path = std::filesystem::relative(loc_path, svc_dir.parent_path());
			try {
				if (std::filesystem::is_directory(loc_path)) {
					// Create obj for each child of loc
					std::vector<std::string> children;
					for (const auto & subfile : std::filesystem::directory_iterator(loc_path)) {
						// Ignore SVC_DIR (and .git for now)
						if (subfile.path() == svc_dir || subfile.path().filename() == ".git") continue;
						auto new_path = this->dfs_add(subfile, staging_dict);
						children.push_back(new_path.parent_path().filename().string() + new_path.filename().string());
					}

					// Update staging entry for filename
					sort(children.begin(), children.end());
					std::ostringstream concat;
					for (auto it = children.begin(); it != children.end(); it++) {
						concat << *it;
					}
					auto res = Objects::create_obj(loc_path, sha256(concat.str()), &children);
					std::string filehash = res.parent_path().filename().string() + res.filename().string();
					if (staging_dict.count(relative_path.string()) == 0 || staging_dict[relative_path.string()] != filehash) {
						staging_dict[relative_path.string()] = filehash;
						updated_dict = true;
					}
				} else {
					auto obj_path = Objects::create_obj(loc_path, sha256_file(loc_path), NULL);
					std::string filehash = obj_path.parent_path().filename().string() + obj_path.filename().string();
					if (staging_dict.count(relative_path.string()) == 0 || staging_dict[relative_path.string()] != filehash) {
						staging_dict[relative_path.string()] = filehash;
						updated_dict = true;
					}
				}
			} catch (std::string err) {
				ERR(err);
				return;
			}
		}

		// If staging dict was changed, write to STAGING
		if (updated_dict) Dict::write_dict_file("STAGING", staging_dict);
		else std::cout << "Nothing to add." << std::endl;
	}

	std::filesystem::path Add::dfs_add(const std::filesystem::directory_entry& entry, std::unordered_map<std::string, std::string>& dict) {
		// If directory, object needs to be created or linked with children
		if (entry.is_directory()) {
			std::vector<std::string> children;

			// Recursively handle children first and sort them by name in ascending order
			for (const auto & ch : std::filesystem::directory_iterator(entry)) {
				auto ch_path = this->dfs_add(ch, dict);
				children.push_back(ch_path.parent_path().filename().string() + ch_path.filename().string());
			}
			std::sort(children.begin(), children.end());

			// Concatenate filename with child names, and use that string for hash
			std::ostringstream concat;
			concat << entry.path().string();
			for (size_t i = 0; i < children.size(); i++) {
				concat << children[i];
			}

			auto filehash = sha256(concat.str());
			auto res = Objects::create_obj(entry.path(), filehash, &children);

			// Update dict entry
			dict[entry.path().string()] = filehash;

			this->wrote_new = Repo::created_new;
			return res;
		} else {
			// Not a directory, directly create object
			auto filehash = sha256_file(entry.path());
			auto res = Objects::create_obj(entry.path(), filehash, nullptr);
			
			// Update dict entry
			dict[entry.path().string()] = filehash;
			
			this->wrote_new = Repo::created_new;
			return res;
		}
	}

	void Add::help() {
		std::cout << "add\n";
		std::cout << "Description: Add files and directories to have changes committed to next version.\n";
		std::cout << "Usage: svc add <loc1> <loc2> ...\n";
		std::cout << "Params:\n\tloc<n>: File/directory to track\n";
	}
}
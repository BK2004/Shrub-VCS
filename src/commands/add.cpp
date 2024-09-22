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

		DirTrie::DirTrie<bool>* add_trie = new DirTrie::DirTrie<bool>(svc_dir.parent_path(), false); // Store T/F to indicate whether file was directly added
		std::unordered_map<std::string, std::string> staging_dict;
		DirTrie::DirTrie<std::string>* staging_trie;
		try {
			staging_dict = Dict::read_dict_file("STAGING");
			staging_trie = Dict::build_trie(staging_dict);
		} catch (std::string e) {
			ERR(e);
			delete add_trie;
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
				delete add_trie; delete staging_trie;
				return;
			}

			// Ignore SVC_DIR (and .git for now)
			if (loc_path == svc_dir || loc_path.filename() == ".git") continue;

			auto relative_path = std::filesystem::relative(loc_path, svc_dir.parent_path());
			try {
				add_trie->insert(relative_path, true, false);
			} catch (std::string e) {
				ERR(e);
				delete staging_trie;
				delete add_trie;
				return;
			}
		}


		std::string staging_dict_file = "STAGING";
		auto root_dir = svc_dir.parent_path();
		this->populate_add(add_trie);
		staging_trie = this->update_staging(add_trie, staging_trie, root_dir);
		Dict::write_trie_to_dict(staging_trie, staging_dict_file);

		delete add_trie; delete staging_trie;
	}

	// populate_add
	// 	Populates add trie with descendants of locations that are being added
	// Params:
	// 	add_trie: add trie
	// Returns: N/A
	void Add::populate_add(DirTrie::DirTrie<bool>* add_trie) {
		if (add_trie == NULL) return;
		// Base cases:
		//	add_trie is set to true: add all descendants to add_trie
		if (add_trie->get_val()) {
			auto node_path = add_trie->get_root();
			
			if (std::filesystem::is_directory(node_path)) {
				for (auto entry : std::filesystem::directory_iterator(node_path)) {
					if (entry.path().filename().string() == ".git" || entry.path().filename().string() == SVC_DIR) continue;

					std::filesystem::path ch_path = entry.path();
					if (add_trie->get_children()->count(ch_path.filename().string()) == 0) {
						add_trie->insert(ch_path, true, true);
					} else {
						add_trie->get_children()->at(ch_path.filename().string())->set_val(true);
					}

					// std::cout << ch_path.string() << ": " << add_trie->get_children()->at(ch_path.filename().string())->get_val() << std::endl;
					populate_add(add_trie->get_children()->at(ch_path.filename().string()));
				}
			}
		}

		// add_trie is false
		// recurse on children of add_trie
		for (auto it = add_trie->get_children()->begin(); it != add_trie->get_children()->end(); it++) {
			populate_add(it->second);
		}
	}

	// update_staging
	// 	Update staging trie with information from add_trie
	// Params:
	// 	add_trie: Trie that contains whether files/dir are being added (changed) or not
	// 	staging_trie: Trie that will be committed to STAGING
	// Returns:
	// 	Pointer to staging trie node or nullptr if none
	DirTrie::DirTrie<std::string>* Add::update_staging(DirTrie::DirTrie<bool>* add_trie, DirTrie::DirTrie<std::string>* staging_trie, std::filesystem::path& root_dir) {
		// Base case, null add_trie
		if (add_trie == NULL) return NULL;

		// If staging trie is null
		if (staging_trie == NULL) {
			// Create new one with add trie root
			staging_trie = new DirTrie::DirTrie<std::string>(add_trie->get_root(), "");
		}

		// If add trie val is true, this is overwriting current staging trie
		if (add_trie->get_val()) {
			// Replace staging_trie
			delete staging_trie;
			staging_trie = new DirTrie::DirTrie<std::string>(add_trie->get_root(), "");

			// Recurse on children of add trie
			for (auto it = add_trie->get_children()->begin(); it != add_trie->get_children()->end(); it++) {
				(*(staging_trie->get_children()))[it->first] = update_staging(it->second, NULL, root_dir);
			}
		}

		// If add_trie val is false, no children removed
		else {
			// Recurse on children of add trie
			for (auto it = add_trie->get_children()->begin(); it != add_trie->get_children()->end(); it++) {
				(*(staging_trie->get_children()))[it->first] = staging_trie->get_children()->count(it->first) == 0
					? update_staging(it->second, NULL, root_dir)
					: update_staging(it->second, staging_trie->get_children()->at(it->first), root_dir);
			}
		}

		std::filesystem::path new_obj;
		if (staging_trie->get_children()->size() == 0) {
			// No children, just a file
			new_obj = Objects::create_obj(staging_trie->get_root(), sha256_file(staging_trie->get_root()), NULL);
		} else {
			// Directory, collect all child hashes
			std::vector<std::string> children;
			for (auto it = staging_trie->get_children()->begin(); it != staging_trie->get_children()->end(); it++) {
				children.push_back(it->second->get_val());
			}
			std::sort(children.begin(), children.end());

			std::ostringstream concat;
			concat << std::filesystem::relative(staging_trie->get_root(), root_dir).string() << std::endl;

			for (size_t i = 0; i < children.size(); i++) {
				concat << children[i] << std::endl;
			}

			// Hash with concatted children
			new_obj = Objects::create_obj(staging_trie->get_root(), sha256(concat.str()), &children);
		}

		staging_trie->set_val(new_obj.parent_path().filename().string() + new_obj.filename().string());

		return staging_trie;
	}


	// dfs_hash
	// 	Hash filepath and all of its children
	// Params:
	// 	path: Path to file/directory to hash
	// Returns:
	// 	Filepath of hashed object
	std::filesystem::path Add::dfs_hash(std::filesystem::path path, std::unordered_map<std::string, std::string>& staging_dict) {
		auto svc_path = Repo::get_svc_dir();
		auto relative_path = std::filesystem::relative(path, svc_path.parent_path());
		if (std::filesystem::is_regular_file(path)) {
			return Objects::create_obj(path, sha256_file(path), NULL);
		} else {
			std::vector<std::string> children;
			// is a directory
			for (auto ch : std::filesystem::directory_iterator(path)) {
				auto ret = dfs_hash(ch.path(), staging_dict);

				children.push_back(ret.parent_path().filename().string() + ret.filename().string());
			}

			std::sort(children.begin(), children.end());
			std::ostringstream concat;
			concat << relative_path.string() << std::endl;
			for (size_t i = 0; i < children.size(); i++) {
				concat << children[i] << std::endl;
			}

			auto filehash = sha256(concat.str());
			auto res = Objects::create_obj(relative_path, filehash, &children);

			staging_dict[relative_path.string()] = filehash;
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
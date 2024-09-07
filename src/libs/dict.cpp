#include "dict.h"

namespace Dict {
	// read_dict_file
	// 	Reads dict file into unordered map of filenames/directories to hashes
	// 	Throws string error if not tracking
	// 	dict stored as:
	// 		<#chars in filename>:<filename1><hash1>
	// 		<#chars in filename>:<filename2><hash2>
	// 		...
	// Params:
	// 	dict_filename: Name of dictionary file
	// Returns:
	// 	Unordered map of filenames and file hashes
	std::unordered_map<std::string, std::string> read_dict_file(std::string dict_filename) {
		auto svc_dir = Repo::get_svc_dir();
		std::unordered_map<std::string, std::string> out;
		if (svc_dir.empty()) throw "Not tracking directory (read_dict_file:1)";

		std::filesystem::path dict_path = svc_dir / dict_filename;
		if (!std::filesystem::exists(dict_path)) return out;

		// Dict path exists, so read entries into out
		std::ifstream dict_stream(dict_path);
		if (!dict_stream) return out;

		while (dict_stream) {
			char add_or_remove;
			char digit;
			int len_filename = 0;

			dict_stream >> digit;
			if (!dict_stream) return out;

			while (digit != ':') {
				len_filename = 10 * len_filename + (digit - '0');
				dict_stream >> digit;
			}

			char* filename = new char[len_filename + 1]{'\0'};
			std::string filehash;

			dict_stream.read(filename, len_filename);
			if (dict_stream.gcount() < len_filename) throw "Failed to retrieve files in dict (read_dict_file:2)";
			dict_stream >> filehash;

			out[std::string(filename)] = filehash;

			delete filename;
		}

		return out;
	}

	// write_dict_file
	// 	Writes dict to dict file
	// 	dict stored as:
	// 		<#chars in filename>:<filename1><hash1>
	// 		<#chars in filename>:<filename2><hash2>
	// 		...
	// Params:
	// 	dict_filename: Dict file to write to
	// 	file_hashes: Map of files to hashes to use as basis
	// Returns: N/A
	void write_dict_file(std::string dict_filename, std::unordered_map<std::string, std::string>& file_hashes) {
		auto svc_dir = Repo::get_svc_dir();
		if (svc_dir.empty()) throw "Not tracking directory (write_dict_file:1)";

		std::ofstream dict_file(svc_dir / dict_filename);
		if (!dict_file) throw "Failed to write to " + dict_filename + " (write_dict_file:2)";

		for (auto it = file_hashes.begin(); it != file_hashes.end(); it++) {
			dict_file << it->first.size() << ":" << it->first << it->second << std::endl;
		}
	}

	// build_trie
	// 	Builds DirTrie from dict
	// 	dict stored as:
	// 		<#chars in filename>:<filename1><hash1>
	// 		<#chars in filename>:<filename2><hash2>
	// 		...
	// Params:
	// 	dict: Dict of filenames and hashes
	// Returns:
	// 	DirTrie
	DirTrie::DirTrie<std::string>* build_trie(std::unordered_map<std::string, std::string>& dict) {
		auto svc_dir = Repo::get_svc_dir();
		if (svc_dir.empty()) throw "Directory not being tracked.";
		auto src_dir = svc_dir.parent_path();
		auto res = new DirTrie::DirTrie<std::string>(src_dir, "");

		for (auto it = dict.begin(); it != dict.end(); it++) {
			auto new_path = src_dir / it->first;
			res->insert(new_path, it->second, "");
		}

		return res;
	}

	// write_trie_to_dict
	// 	Writes DirTrie to dict file
	// 	dict stored as:
	// 		<#chars in filename>:<filename1><hash1>
	// 		<#chars in filename>:<filename2><hash2>
	// 		...
	// Params:
	// 	trie: DirTrie
	// 	dict_filename: Name of dict file
	// Returns: N/A
	void write_trie_to_dict(DirTrie::DirTrie<std::string>* trie, std::string& dict_filename) {
		auto svc_dir = Repo::get_svc_dir();
		if (svc_dir.empty()) throw "Not tracking directory (write_trie_to_dict:1)";

		std::ofstream dict_file(svc_dir / dict_filename);
		if (!dict_file) throw "Failed to write to " + dict_filename + " (write_trie_to_dict:2)";

		std::stack<DirTrie::DirTrie<std::string>*> dfs;
		dfs.push(trie);

		while (!dfs.empty()) {
			auto dirtrie = dfs.top(); dfs.pop();
			if (!dirtrie->get_val().empty()) {
				// Write to dict file
				auto root_path = std::filesystem::relative(dirtrie->get_root(), svc_dir.parent_path());
				dict_file << root_path.string().size() << ":" << root_path.string() << dirtrie->get_val() << std::endl;
			}
			for (auto it = dirtrie->get_children()->begin(); it != dirtrie->get_children()->end(); it++) {
				dfs.push(it->second);
			}
		}
	}
}
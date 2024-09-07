#include "command_utils.h"
#include <stdio.h>

namespace Commands {
	bool created_new = false;

	// get_svc_dir
	// 	Get path of .svc directory in current directory
	// Params:
	// Returns:
	// 	Path to SVC_DIR if it exists, otherwise an empty path
	std::filesystem::path get_svc_dir() {
		std::filesystem::path dir = std::filesystem::current_path();
		while (dir.string() != dir.root_path() && dir.has_parent_path()) {
			if (std::filesystem::exists(dir.string() + "\\" + SVC_DIR)) {
				return std::filesystem::path(dir.string() + "\\" + SVC_DIR);
			} else {
				dir = dir.parent_path();
			}
		}

		dir.clear();
		return dir;
	}

	// update_ref
	// 	Update reference under .svc/refs
	// Params:
	// 	ref_name: Name of reference to update
	// 	value: New value of reference
	// Returns:
	void update_ref(const char* ref_name, std::string value) {
		auto svc_dir = get_svc_dir();
		if (svc_dir.empty()) return;

		std::filesystem::path ref_path = svc_dir / "refs"/ ref_name;
		std::ofstream ref_ofs(ref_path);
		if (!ref_ofs) return;

		ref_ofs << value;
		ref_ofs.close();
	}

	// get_ref
	// 	Get value of ref
	// Params:
	// 	ref_name: Name of reference to get
	// Returns:
	// 	Value of ref
	std::string get_ref(const char* ref_name) {
		auto svc_dir = get_svc_dir();
		if (svc_dir.empty()) return "";

		auto ref_path = svc_dir / "refs" / ref_name;
		std::ifstream ref(ref_path);
		if (!ref) return "";

		std::string out;
		ref >> out;

		ref.close();
		return out;
	}

	// create_obj
	// 	Create file under 'objects' from loc with filename name
	// 	Link children to file inside
	// 	After compression, obj file is formatted as follows:
	// 		<filepath>
	// 		<# children> <child 1>
	// 		<child 2>
	// 		...
	// 		<child n>
	// 		file contents (compressed)
	// Params:
	// 	loc - Location of original file
	// 	name - SHA256 filename
	// 	children - List of child names if this is a directory
	// Returns:
	// 	Path to object file if successful
	// 	zerror / empty path if failed
	std::filesystem::path create_obj(std::filesystem::path loc, std::string name, std::vector<std::string>* children) {
		created_new = false;
		std::filesystem::path obj_dir = get_svc_dir() / "objects";
		if (obj_dir.parent_path().filename() != SVC_DIR) return "";

		// Create folder to hold new obj under .svc/objects (e.g., .svc/objects/o7/lsdfK3...)
		char obj_folder[3] = {name[0], name[1]};
		std::filesystem::create_directory(obj_dir / obj_folder);
		std::filesystem::path dest_path = obj_dir / obj_folder / (name.substr(2));
		if (std::filesystem::exists(dest_path)) {
			return dest_path;
		}
		
		std::ofstream dest_ofstream(dest_path);
		if (!dest_ofstream) throw "Couldn't open " + dest_path.string();

		// Write filename, children hashes, etc. to header
		std::string filename = std::filesystem::canonical(loc).lexically_relative(obj_dir.parent_path().parent_path()).string();
		dest_ofstream << filename << std::endl;
		if (children) {
			dest_ofstream << children->size() << " ";
			for (auto it = children->begin(); it != children->end(); it++) {
				dest_ofstream << it->data() << std::endl;
			}
		} else {
			dest_ofstream << "0 ";
		}
		dest_ofstream.close();

		// Compress until EOF if src is a file
		if (!std::filesystem::is_regular_file(loc)) {
			created_new = true;
			return dest_path;
		}

		FILE* src = fopen(loc.string().c_str(), "rb");
		if (ferror(src)) {
			std::filesystem::remove(dest_path);
			throw "Couldn't read " + loc.string();
		}

		FILE* dest = fopen(dest_path.string().c_str(), "a+");
		if (ferror(dest)) {
			fclose(src);
			throw "Couldn't write to " + dest_path.string();
		}

		// Deflate src into dest
		int ret, flush;
		unsigned have;
		z_stream strm;
		unsigned char in[CHUNK] = {'\0'};
		unsigned char out[CHUNK] = {'\0'};

		strm.zalloc = NULL;
		strm.zfree = NULL;
		strm.opaque = NULL;
		ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
		if (ret != Z_OK) {
			std::filesystem::remove(dest_path);
			fclose(src);
			fclose(dest);
			throw "Failed to deflate " + loc.string();
		}

		do {
			strm.avail_in = fread(in, 1, CHUNK, src);
			if (ferror(src)) {
				(void)deflateEnd(&strm);
				fclose(src);
				fclose(dest);
				throw "Failed to deflate " + dest_path.string();
			}
			flush = feof(src) ? Z_FINISH : Z_NO_FLUSH;
			strm.next_in = in;

			do {
				strm.avail_out = CHUNK;
				strm.next_out = out;
				ret = deflate(&strm, flush);
				assert(ret != Z_STREAM_ERROR);
				have = CHUNK - strm.avail_out;
				if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
					(void)deflateEnd(&strm);
					fclose(src);
					fclose(dest);
					throw "Failed to deflate " + dest_path.string();
				}
			} while (strm.avail_out == 0);
			assert(strm.avail_in == 0);

		} while (flush != Z_FINISH);
		assert(ret == Z_STREAM_END);

		deflateEnd(&strm);
		fclose(src);
		fclose(dest);
		created_new = true;
		return dest_path;
	}

	// create_commit_obj
	// 	Create commit object file and update head ref
	// 	Commit file laid out as such:
	// 		<length of message> <message>
	// 		<commit date/time>
	// 		<# children> <child 1>
	// 		<child 2>
	// 		...
	// 		<child n>
	// 		<has previous commit (T/F)><previous commit id?>
	// Params:
	// 	message: commit message
	// 	children: "children" of commit, i.e., updated root files in commit
	// 	prev: Previous commit id
	// Returns:
	// 	Path to commit object file
	std::filesystem::path create_commit_obj(std::string& message, std::vector<std::string>& children, std::string* prev) {
		std::filesystem::path obj_dir = get_svc_dir() / "objects";
		if (obj_dir.parent_path().filename() != SVC_DIR) return "";

		std::ostringstream commit_contents;
		commit_contents << message.size() << " " << message << std::endl;

		// Get current date/time
		time_t timestamp;
		time(&timestamp);
		char buf[sizeof "2011-10-08T07:07:09Z"];
		strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&timestamp));

		commit_contents << buf << std::endl;

		// Output child files to commit contents
		commit_contents << children.size() << " ";
		for (size_t i = 0; i < children.size(); i++) {
			commit_contents << children[i] << std::endl;
		}

		if (prev == NULL) {
			commit_contents << "F";
		} else {
			commit_contents << "T" << *prev;
		}

		std::string hashed_commit = sha256(commit_contents.str());
		update_ref("head", hashed_commit);

		// Write commit details to file
		// Create folder to hold new obj under .svc/objects (e.g., .svc/objects/o7/lsdfK3...)
		char obj_folder[3] = {hashed_commit[0], hashed_commit[1]};
		std::filesystem::create_directory(obj_dir / obj_folder);
		std::filesystem::path dest_path = obj_dir / obj_folder / (hashed_commit.substr(2));

		std::ofstream dest(dest_path);
		if (!dest) { return ""; }

		dest << commit_contents.str();
		dest.close();
		commit_contents.clear();

		LOG("commits/head", hashed_commit << ": " << buf << std::endl << "\"" << message << "\"" << std::endl);

		return dest_path;
	}

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
		auto svc_dir = get_svc_dir();
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
	// Params:
	// 	dict_filename: Dict file to write to
	// 	file_hashes: Map of files to hashes to use as basis
	// Returns: N/A
	void write_dict_file(std::string dict_filename, std::unordered_map<std::string, std::string>& file_hashes) {
		auto svc_dir = get_svc_dir();
		if (svc_dir.empty()) throw "Not tracking directory (write_dict_file:1)";

		std::ofstream dict_file(svc_dir / dict_filename);
		if (!dict_file) throw "Failed to write to " + dict_filename + " (write_dict_file:2)";

		for (auto it = file_hashes.begin(); it != file_hashes.end(); it++) {
			dict_file << it->first.size() << ":" << it->first << it->second << std::endl;
		}
	}
}
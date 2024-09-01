#include "command_utils.h"
#include <stdio.h>

namespace Commands {
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
		if (!std::filesystem::is_regular_file(loc)) return dest_path;

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
		return dest_path;
	}
}
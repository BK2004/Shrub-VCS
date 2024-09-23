#include "repo.h"

namespace Repo {
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

	/** 
	 * Get value of ref
	 * @param ref_name: Name of reference to get
	 * @returns Value of ref, or "" if it doesn't exist
	 */
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
}
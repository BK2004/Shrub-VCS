#ifndef DIRTRIE_H
#define DIRTRIE_H
#include <filesystem>
#include <string>
#include <unordered_map>
#include <stack>
#include <queue>

namespace DirTrie {
	template <typename T>
	class DirTrie {
	public:
		DirTrie<T>(std::filesystem::path root, T val) : root(root), val(val) {}
		~DirTrie<T>() {
			for (auto it = this->children.begin(); it != this->children.end(); it++) {
				delete it->second;
			}
		}
		
		// insert
		// 	Recursive function to insert node with val 'val' into Trie
		// 	Creates necessary nodes along the way
		// Params:
		// 	path: Path to node that is being inserted
		// 	val: Value of new node
		// Returns: N/A
		void insert(std::filesystem::path& path, T val, T default_val) {
			// Separate path into its part
			auto path_stack = separate_path(path);
			std::cout << "Separated into " << path_stack.size() << " layers\n";

			this->insert_helper("", path_stack, val, default_val);
		}

		T get_val(std::filesystem::path& path) const {
			auto path_stack = separate_path(path);

			return this->get_node(path_stack)->get_val();
		}

		void set_val(std::filesystem::path& path, T val) {
			auto path_stack = separate_path(path);

			this->get_node(path_stack)->set_val(val);
		}

		void print_trie() {
			std::queue<std::pair<DirTrie<T>*, int>> bfs;
			std::pair<DirTrie<T>*, int> init = {this, 0};
			bfs.push(init);
			int i = 0;
			std::cout << "0: ";
			while (!bfs.empty()) {
				std::pair<DirTrie<T>*, int> curr = bfs.front(); bfs.pop();
				if (curr.second != i) {
					i++;
					std::cout << i << ":\n";
				}
				std::cout << "\t" << curr.first->root.string() << " = " << curr.first->get_val() << std::endl;

				for (auto it = curr.first->children.begin(); it != curr.first->children.end(); it++) {
					bfs.push({it->second, curr.second + 1});
				}
			}
		}

		T get_val() const { return this->val; }
		void set_val(T val) { this->val = val; }
	private:
		// Separate path into stack containing folders of path
		std::stack<std::string> separate_path(std::filesystem::path& path) {
			std::stack<std::string> stack;
			auto curr = std::filesystem::absolute(path);

			int i = 0;
			int root_len = this->root.string().size();
			while (!curr.empty() && curr.string().size() > root_len && curr != this->root) {
				if (curr.filename().string() != ".")
					stack.push(curr.filename().string());
				curr = curr.parent_path();
			}

			if (curr.string().size() < root_len) throw path.string() + " not in the root directory";
			return stack;
		}

		void insert_helper(std::filesystem::path path, std::stack<std::string>& path_stack, T& val, T& default_val) {
			// base case, no more path to traverse, set val
			if (path_stack.empty()) {
				this->set_val(val);
				return;
			}

			std::string top = path_stack.top(); path_stack.pop();
			path = path / top;
			if (this->children.count(top) == 0) {
				// Create new child with top segment
				auto child = new DirTrie<T>(path, default_val);
				this->children[top] = child;
			}

			return this->children[top]->insert_helper(path, path_stack, val, default_val);
		}

		DirTrie<T>* get_node(std::stack<std::string>& path_stack) const {
			if (path_stack.empty()) return this;

			auto top = path_stack.top(); path_stack.pop();
			if (this->children.count(top) == 0) throw "Path not in dirtrie";

			return this->children[top]->get_node(path_stack);
		}

		std::filesystem::path root;
		T val;
		std::unordered_map<std::string, DirTrie<T>*> children;
	};
}

#endif
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include <memory>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
private:
	
	struct AVL_node;
	
	typedef std::shared_ptr<AVL_node> Node_Point;
	
	struct AVL_node{
		pair<const Key, T> node;
		Node_Point ls, rs;
		int level;
		AVL_node(const pair<const Key, T> &rhs) : node(rhs), level(0) {
			ls = nullptr; rs = nullptr;
		}
	};
	Compare comp;
	Node_Point root;
	int sum;
	
	int max(int a, int b) {return a > b ? a : b;}
	
	inline int H(Node_Point p) {return p == nullptr ? -1 : p -> level;}
	
	void LL(Node_Point &t){
		Node_Point now = std::shared_ptr<AVL_node>(new AVL_node(*t));
		t = now;
		Node_Point k = std::shared_ptr<AVL_node>(new AVL_node(*(t -> ls)));
		t -> ls = k -> rs;
		k -> rs = t;
		t -> level = max(H(t -> ls), H(t -> rs)) + 1;
		k -> level = max(H(k -> ls), H(k -> rs)) + 1;
		t = k;
	}
	
	void RR(Node_Point &t){
		Node_Point now = std::shared_ptr<AVL_node>(new AVL_node(*t));
		t = now;
		Node_Point k = std::shared_ptr<AVL_node>(new AVL_node(*(t -> rs)));
		t -> rs = k -> ls;
		k -> ls = t;
		t -> level = max(H(t -> ls), H(t -> rs)) + 1;
		k -> level = max(H(k -> ls), H(k -> rs)) + 1;
		t = k;
	}
	
	void LR(Node_Point &t){
		RR(t -> ls); LL(t);
	}
	
	void RL(Node_Point &t){
		LL(t -> rs); RR(t);
	}
	
	Node_Point AVL_Find(const Key &key, Node_Point t) const{
		if(t == nullptr) return nullptr;
		if(comp(key, (t -> node).first)){
			return AVL_Find(key, t -> ls);
		} else
		if(comp((t -> node).first, key)){
			return AVL_Find(key, t -> rs);
		}
		else return t;
	}
	
	Node_Point AVL_Find_copy(const Key &key, Node_Point &t) {
		if(t == nullptr) return nullptr;
		Node_Point now = std::shared_ptr<AVL_node>(new AVL_node(*t));
		t = now;
		if(comp(key, (t -> node).first)){
			return AVL_Find_copy(key, t -> ls);
		} else
		if(comp((t -> node).first, key)){
			return AVL_Find_copy(key, t -> rs);
		}
		else return t;
	}
	
	void AVL_Insert(const pair<const Key, T> &X, Node_Point &t){
		if(t != nullptr){
			Node_Point now = std::shared_ptr<AVL_node>(new AVL_node(*t));
			t = now;
		}
		if(t == nullptr){
			t = std::shared_ptr<AVL_node>(new AVL_node(pair<const Key, T>(X)));
			return;
		} else
		if(comp(X.first, (t -> node).first)){
			AVL_Insert(X, t -> ls);
			if(H(t -> ls) - H(t -> rs) == 2){
				if(comp(X.first, (t -> ls -> node).first)) LL(t); else LR(t);
			}
		} else
		if(comp((t -> node).first, X.first)){
			AVL_Insert(X, t -> rs);
			if(H(t -> rs) - H(t -> ls) == 2){
				if(comp((t -> rs -> node).first, X.first)) RR(t); else RL(t);
			}
		}
		else return;
		t -> level = max(H(t -> ls), H(t -> rs)) + 1;
	}
	
	void AVL_Ins(const pair<const Key, T> &X){
		AVL_Insert(X, root);
	}
	
	void AVL_Delete(const Key &key, Node_Point &t){
		if(t == nullptr) return;
		Node_Point now = std::shared_ptr<AVL_node>(new AVL_node(*t));
		t = now;
		if(comp(key, (t -> node).first)){
			AVL_Delete(key, t -> ls); 
			if(H(t -> rs) - H(t -> ls) == 2){
				if(t -> rs -> ls != nullptr && (H(t-> rs ->ls) > H(t -> rs -> rs))) RL(t);
            	else RR(t);
			}
		} else
		if(comp((t -> node).first, key)){
			AVL_Delete(key, t -> rs);
			if(H(t -> ls) - H(t -> rs) == 2){
	            if(t -> ls -> rs != nullptr && (H(t -> ls -> rs) > H(t -> ls -> ls))) LR(t);
	            else LL(t);
			}
		} else
		if(t -> ls != nullptr && t -> rs != nullptr){
			Node_Point k = t -> rs, now;
			while(k -> ls != nullptr) k = k -> ls;
			now = std::shared_ptr<AVL_node>(new AVL_node(k -> node));
			now -> ls = t -> ls; now -> rs = t -> rs; now -> level = t -> level;
			t = now;
			AVL_Delete((k -> node).first, t -> rs);
			if(H(t -> ls) - H(t -> rs) == 2){
                if(t -> ls -> rs != nullptr && (H(t -> ls -> rs) > H(t -> ls -> ls))) LR(t);
                else LL(t);
            }
		}
		else{
			t = (t -> ls != nullptr) ? t -> ls : t -> rs;
			return;
		}
		if(t == nullptr) return;
		t -> level = max(H(t -> ls), H(t -> rs)) + 1;
	}
	
	void AVL_remove(const Key &key){
		AVL_Delete(key, root);
	}
	
public:
	typedef pair<const Key, T> value_type;
	 
	class const_iterator;
	class iterator {
		friend class map;
		Node_Point p;
		map *root;
		Compare cmp;
		Node_Point Find_Pre(Node_Point t) {
			if(t == p) return nullptr;
			Node_Point res = nullptr;
			if(cmp((p -> node).first, (t -> node).first)) res = Find_Pre(t -> ls);
			else{
				res = Find_Pre(t -> rs);
				if(res == nullptr || cmp((res -> node).first, (t -> node).first)) res = t;
			}
			return res;
		}
		
		Node_Point Get_Pre() {
			Node_Point res = Find_Pre(root -> root);
			Node_Point k = p -> ls;
			if(k != nullptr){
				while(k -> rs != nullptr) k = k -> rs;
				res = k;
			}
			return res;
		}
		
		Node_Point Find_Next(Node_Point t) {
			if(t == p) return nullptr;
			Node_Point res = nullptr;
			if(cmp((p -> node).first, (t -> node).first)){
				res = Find_Next(t -> ls);
				if(res == nullptr || cmp((t -> node).first, (res -> node).first)) res = t;
			}
			else res = Find_Next(t -> rs);
			return res;
		}
		
		Node_Point Get_Next() {
			Node_Point res = Find_Next(root -> root);
			Node_Point k = p -> rs;
			if(k != nullptr){
				while(k -> ls != nullptr) k = k -> ls;
				res = k;
			}
			return res;
		}
		
	public:
		Node_Point Getp() const {return p;}
		map* Getroot() const {return root;}
		iterator() : p(nullptr), root(NULL) {}
		iterator(const Node_Point P, map * const ROOT) : p(P) , root(ROOT) {}
		iterator(const iterator &other) {
		    p = other.p; root = other.root;
		}
		
		iterator operator++(int) {
			if(p == nullptr) throw invalid_iterator();
			Node_Point P = p;
			p = Get_Next();
			return iterator(P, root);
		}
	
		iterator& operator++() {
			if(p == nullptr) throw invalid_iterator();
			p = Get_Next();
			return *this;
		}

		iterator operator--(int) {
			Node_Point P = p;
			if(p != nullptr){
				Node_Point t = Get_Pre();
				if(t == nullptr) throw invalid_iterator();
				p = t;
			}
			else{
				if(root -> root == nullptr) throw invalid_iterator();
				Node_Point k = root -> root;
				while(k -> rs != nullptr) k = k -> rs;
				p = k;
			}
			return iterator(P, root);
		}
	
		iterator& operator--() {
			if(p != nullptr){
				Node_Point t = Get_Pre();
				if(t == nullptr) throw invalid_iterator();
				p = t;
			}
			else{
				if(root -> root == nullptr) throw invalid_iterator();
				Node_Point k = root -> root;
				while(k -> rs != nullptr) k = k -> rs;
				p = k;
			}
			return *this;
		}
	
		value_type& operator*() {
			p = (root -> AVL_Find_copy(Key((p -> node).first), root -> root));
			return p -> node;
		}
		
		value_type* operator ->() noexcept {
			p = (root -> AVL_Find_copy(Key((p -> node).first), root -> root));
			return &(p -> node);
		}
		
		bool operator==(const iterator &rhs) const {
			return (p == rhs.p && root == rhs.root);
		}
		bool operator==(const const_iterator &rhs) const {
			return (p == rhs.Getp() && root == rhs.Getroot());
		}
	
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}
	};
	class const_iterator {
	private:
		 friend class map;
		 Node_Point p;
		 const map *root;
		 Compare cmp;
		 Node_Point Find_Pre(Node_Point t) {
			if(t == p) return nullptr;
			Node_Point res = nullptr;
			if(cmp((p -> node).first, (t -> node).first)) res = Find_Pre(t -> ls);
			else{
				res = Find_Pre(t -> rs);
				if(res == nullptr || cmp((res -> node).first, (t -> node).first)) res = t;
			}
			return res;
		}
		
		Node_Point Get_Pre() {
			Node_Point res = Find_Pre(root -> root);
			Node_Point k = p -> ls;
			if(k != nullptr){
				while(k -> rs != nullptr) k = k -> rs;
				res = k;
			}
			return res;
		}
		
		Node_Point Find_Next(Node_Point t) {
			if(t == p) return nullptr;
			Node_Point res = nullptr;
			if(cmp((p -> node).first, (t -> node).first)){
				res = Find_Next(t -> ls);
				if(res == nullptr || cmp((t -> node).first, (res -> node).first)) res = t;
			}
			else res = Find_Next(t -> rs);
			return res;
		}
		
		Node_Point Get_Next() {
			Node_Point res = Find_Next(root -> root);
			Node_Point k = p -> rs;
			if(k != nullptr){
				while(k -> ls != nullptr) k = k -> ls;
				res = k;
			}
			return res;
		}
	public:
		Node_Point Getp() const {return p;}
		const map* Getroot() const {return root;}
		const_iterator() : p(nullptr), root(NULL) {}
		const_iterator(const Node_Point P, const map * const ROOT) : p(P), root(ROOT) {}
		const_iterator(const iterator &other) {
			p = other.Getp(); root = other.Getroot();
		}
		const_iterator(const const_iterator &other) {
			p = other.p; root = other.root;
		}
		
		const_iterator operator++(int) {
			if(p == nullptr) throw invalid_iterator();
			Node_Point P = p;
			p = Get_Next();
			return const_iterator(P, root);
		}

		const_iterator& operator++() {
			if(p == nullptr) throw invalid_iterator();
			p = Get_Next();
			return *this;
		}

		const_iterator operator--(int) {
			Node_Point P = p;
			if(p != nullptr){
				Node_Point t = Get_Pre();
				if(t == nullptr) throw invalid_iterator();
				p = t;
			}
			else{
				if(root -> root == nullptr) throw invalid_iterator();
				Node_Point k = root -> root;
				while(k -> rs != nullptr) k = k -> rs;
				p = k;
			}
			return const_iterator(P, root);
		}

		const_iterator& operator--() {
			if(p != nullptr){
				Node_Point t = Get_Pre();
				if(t == nullptr) throw invalid_iterator();
				p = t;
			}
			else{
				if(root -> root == nullptr) throw invalid_iterator();
				Node_Point k = root -> root;
				while(k -> rs != nullptr) k = k -> rs;
				p = k;
			}
			return *this;
		}

		const value_type& operator*() const{
			Node_Point q = (root -> AVL_Find((p -> node).first, root -> root));
			return q -> node;
		}
		value_type const * operator ->() const noexcept {
			Node_Point q = (root -> AVL_Find((p -> node).first, root -> root));
			return &(q -> node);
		}
		bool operator==(const iterator &rhs) const {
			return (p == rhs.Getp() && root == rhs.Getroot());
		}
		bool operator==(const const_iterator &rhs) const {
			return (p == rhs.p && root == rhs.root);
		}

		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}
	};
	
	friend class iterator;
	friend class const_iterator;
	
	map() : sum(0), root(nullptr) {}
	map(const map &other) {
		sum = other.sum;
		if(other.root != nullptr) root = std::shared_ptr<AVL_node>(new AVL_node(*other.root));
		else root = nullptr;
	}

	map &operator=(const map &other) {
		if(this == &other) return *this;
		root = nullptr;
		sum = other.sum;
		if(other.root != nullptr) root = std::shared_ptr<AVL_node>(new AVL_node(*other.root));
		else root = nullptr;
		return *this;
	}

	~map() {
		root = nullptr;
	}

	T & at(const Key &key) {
		Node_Point p = AVL_Find(key, root);
		if(p == nullptr) throw index_out_of_bound();
		return (p -> node).second;
	}
	
	const T & at(const Key &key) const {
		Node_Point p = AVL_Find(key, root);
		if(p == nullptr) throw index_out_of_bound();
		return (p -> node).second;
	}

	T & operator[](const Key &key) {
		Node_Point p = AVL_Find_copy(key, root);
		if(p == nullptr){
			pair<const Key, T> KT(key, T());
			AVL_Ins(KT); 
			p = AVL_Find(key, root);
			sum++;
		}
		return (p -> node).second;
	}

	const T & operator[](const Key &key) const {
		Node_Point p = AVL_Find(key, root);
		if(p == nullptr) throw index_out_of_bound();
		return (p -> node).second;
	}
	
	iterator begin() {
		if(root == nullptr) return iterator(nullptr, this);
		Node_Point p = root;
		while(p -> ls != nullptr) p = p -> ls;
		return iterator(p, this);
	}
	
	const_iterator cbegin() const {
		if(root == NULL) return const_iterator(nullptr, this);
		Node_Point p = root;
		while(p -> ls != nullptr) p = p -> ls;
		return const_iterator(p, this);
	}

	iterator end() {
		return iterator(nullptr, this);
	}
	
	const_iterator cend() const {
		return const_iterator(nullptr, this);
	}

	bool empty() const {
		return sum == 0;
	}

	size_t size() const {
		return sum;
	}

	void clear() {
		root = nullptr; sum = 0;
	}

	pair<iterator, bool> insert(const value_type &value) {
		Node_Point p = AVL_Find(value.first, root);
		if(p != nullptr) return pair<iterator, bool>(iterator(p, this), 0);
		AVL_Ins(value);
		p = AVL_Find(value.first, root);
		sum++;
		return pair<iterator, bool>(iterator(p, this), 1);
	}

	void erase(iterator pos) {
		Node_Point pp = pos.Getp();
		const map *p_root = pos.Getroot();
		if(p_root == NULL || pp == nullptr) throw invalid_iterator();
		if(this != p_root) throw invalid_iterator();
		Key tt((pp -> node).first);
		AVL_remove(tt);
		sum--;
	}

	size_t count(const Key &key) const {
		Node_Point p = AVL_Find(key, root);
		if(p == nullptr) return 0;
		else return 1;
	}
	
	iterator find(const Key &key) {
		if(root == nullptr) return iterator(nullptr, this);
		Node_Point p = AVL_Find(key, root);
		return iterator(p, this);
	}
	
	const_iterator find(const Key &key) const {
		if(root == nullptr) return const_iterator(nullptr, this);
		Node_Point p = AVL_Find(key, root);
		return const_iterator(p, this);
	}
	
};

}

#endif

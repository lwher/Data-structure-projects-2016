#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
private:
	struct AVL_node{
		pair<const Key, T> *node;
		AVL_node *ls, *rs, *pre, *next;
		int level;
		AVL_node(){
			node = NULL; ls = NULL; rs = NULL; pre = NULL; next = NULL; level = 0;
		}
	};
	Compare comp;
	AVL_node *root;
	int sum;
	
	int max(int a, int b) {return a > b ? a : b;}
	
	inline int H(AVL_node *p) {return p == NULL ? -1 : p -> level;}
	
	void LL(AVL_node* &t){
		AVL_node *k = t -> ls;
		t -> ls = k -> rs;
		k -> rs = t;
		t -> level = max(H(t -> ls), H(t -> rs)) + 1;
		k -> level = max(H(k -> ls), H(k -> rs)) + 1;
		t = k;
	}
	
	void RR(AVL_node* &t){
		AVL_node *k = t -> rs;
		t -> rs = k -> ls;
		k -> ls = t;
		t -> level = max(H(t -> ls), H(t -> rs)) + 1;
		k -> level = max(H(k -> ls), H(k -> rs)) + 1;
		t = k;
	}
	
	void LR(AVL_node* &t){
		RR(t -> ls); LL(t);
	}
	
	void RL(AVL_node* &t){
		LL(t -> rs); RR(t);
	}
	
	AVL_node* AVL_Find(const Key &key, AVL_node *t) const{
		if(t == NULL) return NULL;
		if(comp(key, t -> node -> first)){
			return AVL_Find(key, t -> ls);
		} else
		if(comp(t -> node -> first, key)){
			return AVL_Find(key, t -> rs);
		}
		else return t;
	}
	
	void AVL_Find_pre_next(const Key &key, AVL_node *t, AVL_node* &Pre, AVL_node* &Next){
		if(t == NULL) return;
		if(comp(key, t -> node -> first) && (Next == NULL || comp(t -> node -> first, Next -> node -> first))) Next = t;
		if(comp(t -> node -> first, key) && (Pre == NULL || comp(Pre -> node -> first, t -> node -> first))) Pre = t;
		if(comp(key, t -> node -> first)) AVL_Find_pre_next(key, t -> ls, Pre, Next);
		else AVL_Find_pre_next(key, t -> rs, Pre, Next);
	}
	
	AVL_node* AVL_Insert(const pair<const Key, T> &X, AVL_node* &t){
		AVL_node *W; 
		if(t == NULL){
			t = new AVL_node;
			t -> node = new pair<const Key, T>(X.first, X.second);
			return t;
		} else
		if(comp(X.first, t -> node -> first)){
			W = AVL_Insert(X, t -> ls);
			if(H(t -> ls) - H(t -> rs) == 2){
				if(comp(X.first, t -> ls -> node -> first)) LL(t); else LR(t);
			}
		} else
		if(comp(t -> node -> first, X.first)){
			W = AVL_Insert(X, t -> rs);
			if(H(t -> rs) - H(t -> ls) == 2){
				if(comp(t -> rs -> node -> first, X.first)) RR(t); else RL(t);
			}
		}
		else return t;
		t -> level = max(H(t -> ls), H(t -> rs)) + 1;
		return W;
	}
	
	AVL_node* AVL_Ins(const pair<const Key, T> &X){
		AVL_node *Pre = NULL, *Next = NULL;
		AVL_Find_pre_next(X.first, root, Pre, Next);
		AVL_node *W = AVL_Insert(X, root);
		W -> pre = Pre; W -> next = Next;
		if(Pre != NULL) Pre -> next = W;
		if(Next != NULL) Next -> pre = W;
		return W;
	}
	
	void AVL_Delete(const Key &key, AVL_node* &t){
		if(t == NULL) return;
		if(comp(key, t -> node -> first)){
			AVL_Delete(key, t -> ls); 
			if(H(t -> rs) - H(t -> ls) == 2){
				if(t -> rs -> ls != NULL && (H(t-> rs ->ls) > H(t -> rs -> rs))) RL(t);
            	else RR(t);
			}
		} else
		if(comp(t -> node -> first, key)){
			AVL_Delete(key, t -> rs);
			if(H(t -> ls) - H(t -> rs) == 2){
	            if(t -> ls -> rs != NULL && (H(t -> ls -> rs) > H(t -> ls -> ls))) LR(t);
	            else LL(t);
			}
		} else
		if(t -> ls != NULL && t -> rs != NULL){
			AVL_node *k = t -> rs, *now = t;
			if(k -> ls == NULL){
				 t = k; 
				 t -> ls = now -> ls; now -> rs = t -> rs;
				 t -> rs = now;				 
				 t -> level = now -> level;
				 now -> ls = NULL;
			}
			else{
				while(k -> ls -> ls != NULL) k = k -> ls;
				AVL_node *p = k;
				k = k -> ls;
				p -> ls = now;
				t = k; 
				p = now -> rs;
				now -> rs = t -> rs;
				t -> ls = now -> ls; t -> rs = p;
				t -> level = now -> level; 
				now -> ls = NULL;
			}

			if(t -> next != NULL) t -> next -> pre = t;
			
			AVL_Delete(key, t -> rs);
			if(H(t -> ls) - H(t -> rs) == 2){
                if(t -> ls -> rs != NULL && (H(t -> ls -> rs) > H(t -> ls -> ls))) LR(t);
                else LL(t);
            }
		}
		else{
			AVL_node *k = t;
			t = (t -> ls != NULL) ? t -> ls : t -> rs;
			delete k -> node; delete k;
			return;
		}
		if(t == NULL) return;
		t -> level = max(H(t -> ls), H(t -> rs)) + 1;
	}
	
	void AVL_remove(const Key &key){
		AVL_Delete(key, root);
		AVL_node *Pre = NULL, *Next = NULL;
		AVL_Find_pre_next(key, root, Pre, Next);
		if(Pre != NULL) Pre -> next = Next;
		if(Next != NULL) Next -> pre = Pre;
	}
	
	AVL_node* AVL_copy(AVL_node *t, AVL_node* &tmin, AVL_node* &tmax){
		AVL_node *lmin = NULL, *lmax = NULL, *rmin = NULL, *rmax = NULL, *p;
		p = new AVL_node;
		p -> node = new pair<const Key, T>(*(t -> node));
		p -> ls = NULL; p -> rs = NULL; p -> pre = NULL; p -> next = NULL;
		p -> level = t -> level;
		if(t -> ls != NULL){
			p -> ls = AVL_copy(t -> ls, lmin, lmax);
		}
		if(t -> rs != NULL){
			p -> rs = AVL_copy(t -> rs, rmin, rmax);
		}
		if(lmin == NULL) lmin = p;
		if(rmax == NULL) rmax = p;
		if(lmax != NULL){
			p -> pre = lmax; lmax -> next = p;
		}
		if(rmin != NULL){
			p -> next = rmin; rmin -> pre = p;
		}
		tmin = lmin; tmax = rmax;
		return p;
	}
	
	void AVL_Clear(AVL_node *t){
		if(t -> ls != NULL) AVL_Clear(t -> ls);
		if(t -> rs != NULL) AVL_Clear(t -> rs);
		delete (t -> node); delete t;
	}
	
public:
	typedef pair<const Key, T> value_type;
	 
	class const_iterator;
	class iterator {
		AVL_node *p, *root;
	public:
		AVL_node* Getp() const {return p;}
		AVL_node* Getroot() const {return root;}
		iterator() : p(NULL), root(NULL) {}
		iterator(AVL_node *P, AVL_node *ROOT) : p(P) , root(ROOT) {}
		iterator(const iterator &other) {
		    p = other.p; root = other.root;
		}
		
		iterator operator++(int) {
			if(p == NULL) throw invalid_iterator();
			AVL_node *P = p;
			p = p -> next;
			return iterator(P, root);
		}
	
		iterator& operator++() {
			if(p == NULL) throw invalid_iterator();
			p = p -> next;
			return *this;
		}

		iterator operator--(int) {
			AVL_node *P = p;
			if(p != NULL){
				if(p -> pre == NULL) throw invalid_iterator();
				p = p -> pre;
			}
			else{
				if(root == NULL) throw invalid_iterator();
				AVL_node *k = root;
				while(k -> rs != NULL) k = k -> rs;
				p = k;
			}
			return iterator(P, root);
		}
	
		iterator& operator--() {
			if(p != NULL){
				if(p -> pre == NULL) throw invalid_iterator();
				p = p -> pre;
			}
			else{
				if(root == NULL) throw invalid_iterator();
				AVL_node *k = root;
				while(k -> rs != NULL) k = k -> rs;
				p = k;
			}
			return *this;
		}
	
		value_type& operator*() const{
			return *(p -> node);
		}
		
		value_type* operator ->() const noexcept {
			return p -> node;
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
		 AVL_node *p, *root;
	public:
		AVL_node* Getp() const {return p;}
		AVL_node* Getroot() const {return root;}
		const_iterator() : p(NULL), root(NULL) {}
		const_iterator(AVL_node *P, AVL_node *ROOT) : p(P), root(ROOT) {}
		const_iterator(const iterator &other) {
			p = other.Getp(); root = other.Getroot();
		}
		const_iterator(const const_iterator &other) {
			p = other.p; root = other.root;
		}
		
		const_iterator operator++(int) {
			if(p == NULL) throw invalid_iterator();
			AVL_node *P = p;
			p = p -> next;
			return const_iterator(P, root);
		}

		const_iterator& operator++() {
			if(p == NULL) throw invalid_iterator();
			p = p -> next;
			return *this;
		}

		const_iterator operator--(int) {
			AVL_node *P = p;
			if(p != NULL){
				if(p -> pre == NULL) throw invalid_iterator();
				p = p -> pre;
			}
			else{
				if(root == NULL) throw invalid_iterator();
				AVL_node *k = root;
				while(k -> rs != NULL) k = k -> rs;
				p = k;
			}
			return const_iterator(P, root);
		}

		const_iterator& operator--() {
			if(p != NULL){
				if(p -> pre == NULL) throw invalid_iterator();
				p = p -> pre;
			}
			else{
				if(root == NULL) throw invalid_iterator();
				AVL_node *k = root;
				while(k -> rs != NULL) k = k -> rs;
				p = k;
			}
			return *this;
		}

		const value_type& operator*() const{
			return *(p -> node);
		}
		value_type const * operator ->() const noexcept {
			return p -> node;
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

	map() : sum(0), root(NULL) {}
	map(const map &other) {
		sum = other.sum;
		AVL_node *tmax = NULL, *tmin = NULL;
		if(other.root != NULL) root = AVL_copy(other.root, tmax, tmin);
		else root = NULL;
	}

	map &operator=(const map &other) {
		if(this == &other) return *this;
		if(root != NULL) AVL_Clear(root);
		sum = other.sum;
		AVL_node *tmax = NULL, *tmin = NULL;
		if(other.root != NULL) root = AVL_copy(other.root, tmax, tmin);
		else root = NULL;
		return *this;
	}

	~map() {
		if(root != NULL) AVL_Clear(root);
	}

	T & at(const Key &key) {
		AVL_node *p = AVL_Find(key, root);
		if(p == NULL) throw index_out_of_bound();
		return p -> node -> second;
	}
	
	const T & at(const Key &key) const {
		AVL_node *p = AVL_Find(key, root);
		if(p == NULL) throw index_out_of_bound();
		return p -> node -> second;
	}

	T & operator[](const Key &key) {
		AVL_node *p = AVL_Find(key, root);
		if(p == NULL){
			pair<const Key, T> KT(key, T());
			p = AVL_Ins(KT); 
			sum++;
		}
		return p -> node -> second;
	}

	const T & operator[](const Key &key) const {
		AVL_node *p = AVL_Find(key, root);
		if(p == NULL) throw index_out_of_bound();
		return p -> node -> second;
	}
	
	iterator begin() {
		if(root == NULL) return iterator(NULL, root);
		AVL_node *p = root;
		while(p -> ls != NULL) p = p -> ls;
		return iterator(p, root);
	}
	
	const_iterator cbegin() const {
		if(root == NULL) return const_iterator(NULL, root);
		AVL_node *p = root;
		while(p -> ls != NULL) p = p -> ls;
		return const_iterator(p, root);
	}

	iterator end() {
		return iterator(NULL, root);
	}
	
	const_iterator cend() const {
		return const_iterator(NULL, root);
	}

	bool empty() const {
		return sum == 0;
	}

	size_t size() const {
		return sum;
	}

	void clear() {
		if(root != NULL) AVL_Clear(root);
		root = NULL; sum = 0;
	}

	pair<iterator, bool> insert(const value_type &value) {
		AVL_node *p = AVL_Find(value.first, root);
		if(p != NULL) return pair<iterator, bool>(iterator(p, root), 0);
		p = AVL_Ins(value);
		sum++;
		return pair<iterator, bool>(iterator(p, root), 1);
	}

	void erase(iterator pos) {
		AVL_node *p_root = pos.Getroot(), *pp = pos.Getp();
		if(p_root == NULL || pp == NULL) throw invalid_iterator();
		if(root != p_root) throw invalid_iterator();
		pair<const Key, T> *KT = pp -> node;
		Key tt(KT -> first);
		AVL_remove(tt);
		sum--;
	}

	size_t count(const Key &key) const {
		AVL_node *p = AVL_Find(key, root);
		if(p == NULL) return 0;
		else return 1;
	}
	
	iterator find(const Key &key) {
		if(root == NULL) return iterator(NULL, NULL);
		AVL_node *p = AVL_Find(key, root);
		return iterator(p, root);
	}
	
	const_iterator find(const Key &key) const {
		if(root == NULL) return const_iterator(NULL, NULL);
		AVL_node *p = AVL_Find(key, root);
		return const_iterator(p, root);
	}
	
};

}

#endif

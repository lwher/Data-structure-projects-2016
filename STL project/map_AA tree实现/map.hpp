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
	struct AA_node{
		pair<const Key, T> *node;
		AA_node *ls, *rs, *pre, *next;
		int level;
		AA_node(){
			node = NULL; ls = NULL; rs = NULL; pre = NULL; next = NULL; level = 1;
		}
	};
	Compare comp;
	AA_node *root;
	int sum;
	
	void LL(AA_node* &t){
		if(t -> ls != NULL && t -> ls ->level == t -> level){
			AA_node *k = t -> ls;
			t -> ls = k -> rs;
			k -> rs = t;
			t = k;
		}
	}
	
	void RR(AA_node* &t){
		if(t -> rs != NULL && t -> rs -> rs != NULL && t -> rs -> rs -> level == t -> level){
			AA_node *k = t -> rs;
			t -> rs = k -> ls;
			k -> ls = t;
			t = k;
			t -> level++;
		}
	}
	
	AA_node* AA_Find(const Key &key, AA_node *t) const{
		if(t == NULL) return NULL;
		if(comp(key, t -> node -> first)){
			return AA_Find(key, t -> ls);
		} else
		if(comp(t -> node -> first, key)){
			return AA_Find(key, t -> rs);
		}
		else return t;
	}
	
	void AA_Find_pre_next(const Key &key, AA_node *t, AA_node* &Pre, AA_node* &Next){
		if(t == NULL) return;
		if(comp(key, t -> node -> first) && (Next == NULL || comp(t -> node -> first, Next -> node -> first))) Next = t;
		if(comp(t -> node -> first, key) && (Pre == NULL || comp(Pre -> node -> first, t -> node -> first))) Pre = t;
		if(comp(key, t -> node -> first)) AA_Find_pre_next(key, t -> ls, Pre, Next);
		else AA_Find_pre_next(key, t -> rs, Pre, Next);
	}
	
	AA_node* AA_Insert(const pair<const Key, T> &X, AA_node* &t){
		AA_node *W; 
		if(t == NULL){
			t = new AA_node;
			t -> node = new pair<const Key, T>(X.first, X.second);
			return t;
		} else
		if(comp(X.first, t -> node -> first)){
			W = AA_Insert(X, t -> ls);
		} else
		if(comp(t -> node -> first, X.first)){
			W = AA_Insert(X, t -> rs);
		}
		else return t;
		LL(t); RR(t);
		return W;
	}
	
	AA_node* AA_Ins(const pair<const Key, T> &X){
		AA_node *Pre = NULL, *Next = NULL;
		AA_Find_pre_next(X.first, root, Pre, Next);
		AA_node *W = AA_Insert(X, root);
		W -> pre = Pre; W -> next = Next;
		if(Pre != NULL) Pre -> next = W;
		if(Next != NULL) Next -> pre = W;
		return W;
	}
	
	void AA_Delete(const Key &key, AA_node* &t){
		if(t == NULL) return;
		if(comp(key, t -> node -> first)){
			AA_Delete(key, t -> ls);
		} else
		if(comp(t -> node -> first, key)){
			AA_Delete(key, t -> rs);
		} else
		if(t -> ls != NULL && t -> rs != NULL){
			AA_node *k = t -> rs, *now = t;
			if(k -> ls == NULL){
				 t = k; 
				 t -> ls = now -> ls; now -> rs = t -> rs;
				 t -> rs = now;				 
				 t -> level = now -> level;
				 now -> ls = NULL;
			}
			else{
				while(k -> ls -> ls != NULL) k = k -> ls;
				AA_node *p = k;
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
			
			AA_Delete(key, t -> rs);
		}
		else {
			AA_node *k = t;
			t = (t -> ls != NULL) ? t -> ls : t -> rs;
			delete k -> node; delete k;
			return;
		}
		if(t -> ls == NULL || t -> rs == NULL) t -> level = 1;
		else t -> level = (t -> ls -> level < t -> rs -> level ? t ->ls -> level : t -> rs -> level) + 1;
		if(t -> rs != NULL && t -> rs -> level > t -> level) t -> rs -> level = t -> level;
		LL(t);
		if(t -> rs != NULL) LL(t -> rs);
		if(t -> rs != NULL && t -> rs -> rs != NULL) LL(t -> rs -> rs);
		RR(t);
		if(t -> rs != NULL) RR(t -> rs);
	}
	
	void AA_remove(const Key &key){
		AA_Delete(key, root);
		AA_node *Pre = NULL, *Next = NULL;
		AA_Find_pre_next(key, root, Pre, Next);
		if(Pre != NULL) Pre -> next = Next;
		if(Next != NULL) Next -> pre = Pre;
	}
	
	AA_node* AA_copy(AA_node *t, AA_node* &tmin, AA_node* &tmax){
		AA_node *lmin = NULL, *lmax = NULL, *rmin = NULL, *rmax = NULL, *p;
		p = new AA_node;
		p -> node = new pair<const Key, T>(*(t -> node));
		p -> ls = NULL; p -> rs = NULL; p -> pre = NULL; p -> next = NULL;
		p -> level = t -> level;
		if(t -> ls != NULL){
			p -> ls = AA_copy(t -> ls, lmin, lmax);
		}
		if(t -> rs != NULL){
			p -> rs = AA_copy(t -> rs, rmin, rmax);
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
	
	void AA_Clear(AA_node *t){
		if(t -> ls != NULL) AA_Clear(t -> ls);
		if(t -> rs != NULL) AA_Clear(t -> rs);
		delete (t -> node); delete t;
	}
	
public:
	typedef pair<const Key, T> value_type;
	 
	class const_iterator;
	class iterator {
		AA_node *p, *root;
	public:
		AA_node* Getp() const {return p;}
		AA_node* Getroot() const {return root;}
		iterator() : p(NULL), root(NULL) {}
		iterator(AA_node *P, AA_node *ROOT) : p(P) , root(ROOT) {}
		iterator(const iterator &other) {
		    p = other.p; root = other.root;
		}
		
		iterator operator++(int) {
			if(p == NULL) throw invalid_iterator();
			AA_node *P = p;
			p = p -> next;
			return iterator(P, root);
		}
	
		iterator& operator++() {
			if(p == NULL) throw invalid_iterator();
			p = p -> next;
			return *this;
		}

		iterator operator--(int) {
			AA_node *P = p;
			if(p != NULL){
				if(p -> pre == NULL) throw invalid_iterator();
				p = p -> pre;
			}
			else{
				if(root == NULL) throw invalid_iterator();
				AA_node *k = root;
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
				AA_node *k = root;
				while(k -> rs != NULL) k = k -> rs;
				p = k;
			}
			return *this;
		}
	
		value_type& operator*() const{
			return *(p -> node);
		}
		
		value_type* operator ->() noexcept {
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
		 AA_node *p, *root;
	public:
		AA_node* Getp() const {return p;}
		AA_node* Getroot() const {return root;}
		const_iterator() : p(NULL), root(NULL) {}
		const_iterator(AA_node *P, AA_node *ROOT) : p(P), root(ROOT) {}
		const_iterator(const iterator &other) {
			p = other.Getp(); root = other.Getroot();
		}
		const_iterator(const const_iterator &other) {
			p = other.p; root = other.root;
		}
		
		const_iterator operator++(int) {
			if(p == NULL) throw invalid_iterator();
			AA_node *P = p;
			p = p -> next;
			return const_iterator(P, root);
		}

		const_iterator& operator++() {
			if(p == NULL) throw invalid_iterator();
			p = p -> next;
			return *this;
		}

		const_iterator operator--(int) {
			AA_node *P = p;
			if(p != NULL){
				if(p -> pre == NULL) throw invalid_iterator();
				p = p -> pre;
			}
			else{
				if(root == NULL) throw invalid_iterator();
				AA_node *k = root;
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
				AA_node *k = root;
				while(k -> rs != NULL) k = k -> rs;
				p = k;
			}
			return *this;
		}

		const value_type& operator*() const{
			return *(p -> node);
		}
		value_type const * operator ->() noexcept {
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
		AA_node *tmax = NULL, *tmin = NULL;
		if(other.root != NULL) root = AA_copy(other.root, tmax, tmin);
		else root = NULL;
	}

	map &operator=(const map &other) {
		if(this == &other) return *this;
		if(root != NULL) AA_Clear(root);
		sum = other.sum;
		AA_node *tmax = NULL, *tmin = NULL;
		if(other.root != NULL) root = AA_copy(other.root, tmax, tmin);
		else root = NULL;
		return *this;
	}

	~map() {
		if(root != NULL) AA_Clear(root);
	}
	
	T & at(const Key &key) {
		AA_node *p = AA_Find(key, root);
		if(p == NULL) throw index_out_of_bound();
		return p -> node -> second;
	}
	const T & at(const Key &key) const {
		AA_node *p = AA_Find(key, root);
		if(p == NULL) throw index_out_of_bound();
		return p -> node -> second;
	}

	T & operator[](const Key &key) {
		AA_node *p = AA_Find(key, root);
		if(p == NULL){
			pair<const Key, T> KT(key, T());
			p = AA_Ins(KT); 
			sum++;
		}
		return p -> node -> second;
	}
	
	const T & operator[](const Key &key) const {
		AA_node *p = AA_Find(key, root);
		if(p == NULL) throw index_out_of_bound();
		return p -> node -> second;
	}

	iterator begin() {
		if(root == NULL) return iterator(NULL, root);
		AA_node *p = root;
		while(p -> ls != NULL) p = p -> ls;
		return iterator(p, root);
	}
	const_iterator cbegin() const {
		if(root == NULL) return const_iterator(NULL, root);
		AA_node *p = root;
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
		if(root != NULL) AA_Clear(root);
		root = NULL; sum = 0;
	}

	pair<iterator, bool> insert(const value_type &value) {
		AA_node *p = AA_Find(value.first, root);
		if(p != NULL) return pair<iterator, bool>(iterator(p, root), 0);
		p = AA_Ins(value);
		sum++;
		return pair<iterator, bool>(iterator(p, root), 1);
	}

	void erase(iterator pos) {
		AA_node *p_root = pos.Getroot(), *pp = pos.Getp();
		if(p_root == NULL || pp == NULL) throw invalid_iterator();
		if(root != p_root) throw invalid_iterator();
		pair<const Key, T> *KT = pp -> node;
		Key tt(KT -> first);
		AA_remove(tt);
		sum--;
	}

	size_t count(const Key &key) const {
		AA_node *p = AA_Find(key, root);
		if(p == NULL) return 0;
		else return 1;
	}

	iterator find(const Key &key) {
		if(root == NULL) return iterator(NULL, NULL);
		AA_node *p = AA_Find(key, root);
		return iterator(p, root);
	}
	const_iterator find(const Key &key) const {
		if(root == NULL) return const_iterator(NULL, NULL);
		AA_node *p = AA_Find(key, root);
		return const_iterator(p, root);
	}
	
};

}

#endif

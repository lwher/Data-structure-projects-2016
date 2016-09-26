/*
	二项堆
	writen by lwher 
*/

#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <iostream>
#include <vector>
#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	struct heap_node{
		T *node;
		int level;
		heap_node *ls, *rs;
		heap_node(){ level = -1; }
		heap_node(const T &p){
			node = p;
			level = 0;
			ls = NULL; rs = NULL;
		}
	};
	std::vector<heap_node*> root;
	Compare comp;
	int sum;
	inline bool Is_None(heap_node *p){
		return p == NULL;
	}
	void addchild(heap_node *A, heap_node *B){
		heap_node *p = A -> ls;
		if(p == NULL) A -> ls = B;
		else{
			B -> rs = A -> ls;
			A -> ls = B;
		}
	}
	heap_node* merge(heap_node *A, heap_node *B){
		if(Is_None(A)) return B;
		if(Is_None(B)) return A;
		if(comp(*(B -> node), *(A -> node))){
			addchild(A, B); A -> level++; return A;
		} else {
			addchild(B, A); B -> level++; return B;
		}
	}
    void Union(std::vector<heap_node*> &A, std::vector<heap_node*> &B){ // 合并后放进A里 
		int lenA = A.size(), lenB = B.size();
		heap_node *last = NULL;
		int L = lenA > lenB ? lenA : lenB;
		while(A.size() < L) A.push_back(NULL);
		while(B.size() < L) B.push_back(NULL);
		for(int i = 0; i < L; i++){
			if(Is_None(A[i]) && Is_None(B[i])){
				if(!Is_None(last)){
					A[i] = last; last = NULL;
				}
			} else
			if(Is_None(A[i]) && !Is_None(B[i])){
				if(!Is_None(last)){
					last = merge(last, B[i]);
				}
				else A[i] = B[i];
			} else
			if(!Is_None(A[i]) && Is_None(B[i])){
				if(!Is_None(last)){
					last = merge(last, A[i]);
					A[i] = NULL;
				}
			} else {
				if(!Is_None(last)){
					last = merge(last, B[i]);
				}
				else{
					last = merge(A[i], B[i]);
					A[i] = NULL;
				}
			}
		}
		if(!Is_None(last)) A.push_back(last);
	}
	
	heap_node* Heap_Copy(heap_node *X){
		heap_node *p = (heap_node *) malloc(sizeof(heap_node));
		p -> node = new T(*X -> node);
		p -> level = X -> level;
		p -> ls = NULL; p -> rs = NULL;
		if(X -> ls != NULL) p -> ls = Heap_Copy(X -> ls);
		if(X -> rs != NULL) p -> rs = Heap_Copy(X -> rs);
		return p;
	}
	
	void Heap_Delete(heap_node *X){
		if(X -> ls != NULL) Heap_Delete(X -> ls);
		if(X -> rs != NULL) Heap_Delete(X -> rs);
		delete (X -> node); free(X); X = NULL;
	}
	
	void Vec_clear(){
		int L = root.size();
		for(int i = 0; i < L; i++) if(root[i] != NULL){
			Heap_Delete(root[i]);
			root[i] = NULL;
		}	
	}
public:

	int Get_sum(){ return sum; }
	
	priority_queue() : sum(0) {}
	priority_queue(const priority_queue<T, Compare> &other){
		int L = other.root.size();
		for(int i = 0 ; i < L; i++){
			root.push_back(NULL);
			if(other.root[i] != NULL) root[i] = Heap_Copy(other.root[i]);
		}	
		sum = other.sum;	
	}

	~priority_queue() {
		Vec_clear();
	}

	priority_queue<T, Compare> &operator=(const priority_queue<T, Compare> &other) {
		if(this == &other) return *this;
		int L = other.root.size();
		Vec_clear(); root.clear();
		for(int i = 0 ; i < L; i++){
			root.push_back(NULL);
			if(other.root[i] != NULL) root[i] = Heap_Copy(other.root[i]);
		}	
		sum = other.sum;
		return *this;
	}

	const T & top() const {
		if(sum == 0) throw container_is_empty();
		int L = root.size();
		heap_node *res = NULL; 
		for(int i = 0; i < L; i++){
			if(root[i] != NULL){
				if(res == NULL || comp(*(res -> node), *(root[i] -> node))){
					res = root[i];
				}
			}
		}
		return *(res -> node);
	}

	void push(const T &e) {
		std::vector<heap_node*> Q;
		heap_node *p = (heap_node *) malloc(sizeof(heap_node));
		p -> node = new T(e);
		p -> level = 0;
		p -> ls = NULL; p -> rs = NULL;
		Q.push_back(p);
		Union(root, Q);
		sum++;
	}

	void pop() {
		if(sum == 0) throw container_is_empty();
		std::vector<heap_node*> Q;
		int L = root.size();
		for(int i = 0; i < L; i++) Q.push_back(NULL);
		int W_min = -1;
		for(int i = 0; i < L; i++) if(root[i] != NULL){
			if(W_min == -1 || (comp( *(root[W_min] -> node), *(root[i] -> node)))) W_min = i;
		}
		heap_node *p = root[W_min];
		root[W_min] = NULL;
		heap_node *q = p -> ls; 
		while(q != NULL){
			Q[q -> level] = q; heap_node *x = q -> rs; q -> rs = NULL; q = x;
		}
		Union(root, Q);
		sum--;
		delete(p -> node); free(p); p = NULL;
	}

	size_t size() const {
		return sum;
	}

	bool empty() const {
		return sum == 0;
	}
};

}

#endif

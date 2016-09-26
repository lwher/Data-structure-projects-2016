#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu { 

template<class T>
class deque {
private:
	
	int Per_Size, Sqrt;
	class List{
		public:
			int head, tail, Per_Size;
			T** data;
			List *pre, *next;
			int Get_Size() const {return tail - head + 1;} 
			List(int Size) : Per_Size(Size), head(Size), tail(Size - 1), pre(NULL), next(NULL) {
				data = new T* [(Per_Size << 1) + 10];
			}	
	};
	
	int sum;
	List *first, *last;
	
	void rebuild(){
		List *t, *w, *p, *Pre, *Q = first;
		t = new List(Per_Size); p = t;
		p -> head = (Per_Size >> 1); p -> tail = p -> head - 1;
		while(Q != NULL){
			for(int i = Q -> head; i <= Q -> tail; i++){
				if(p -> tail - p -> head + 1 >= Per_Size){
					Pre = p; p = new List(Per_Size);
					p -> head = (Per_Size >> 1); p -> tail = p -> head - 1;
					p -> pre = Pre; Pre -> next = p;
				}
				p -> data[++(p -> tail)] = Q -> data[i];
			}
			w = Q;
			Q = Q -> next;
			delete [] w -> data;
			delete w;
		}
		first = t; last = p;
	}
	
	inline int Get_Ksz(){
		if(sum <= 10) return 3;
		if(sum <= 100) return 10;
		if(sum <= 1000) return 30;
		if(sum <= 10000) return 100;
		return Sqrt;
	}
	
	inline void Add_sum(){
		sum++;
		if((Sqrt + 1) * (Sqrt + 1) <= sum) Sqrt++; 
		int k = Get_Ksz();
		if(k > Per_Size){
			if((Per_Size << 1) <= k){
				Per_Size = k;
				rebuild();
			}
		}
		else{
			if((k << 1) <= Per_Size){
				Per_Size = k;
				rebuild();
			}
		}
	}
	
	inline void Dec_sum(){
		sum--;
		if(Sqrt * Sqrt > sum) Sqrt--;
		int k = Get_Ksz();
		if(k > Per_Size){
			if((Per_Size << 1) <= k){
				Per_Size = k;
				rebuild();
			}
		}
		else{
			if((k << 1) <= Per_Size){
				Per_Size = k;
				rebuild();
			}
		}
	}
	
	void Balance(List *t){ // �� t �е�Ԫ�ؾ��з��� 
		int size = t -> Get_Size(), L = ((Per_Size << 1) + 9 - size) >> 1;
		if(L == t -> head) return;
		if(L < (t -> head)){
			int l = L;
			for(int i = t -> head; i <= t -> tail; i++) t -> data[L++] = t -> data[i];
			t -> head = l; t -> tail = L - 1;
		}
		else{
		    int R = L + size - 1, r = R;
		    for(int i = t -> tail; i >= t -> head; i--) t -> data[R--] = t -> data[i];
		    t -> tail = r; t -> head = R + 1;
		}
	}
	
	void Merge(List *t, List *p){ // �� p �ϲ��� t   p = t > next;
		int Lt = t -> Get_Size(), Lp = p -> Get_Size();
		if(t -> tail + Lp > (Per_Size << 1) + 6){
			int size = Lt + Lp;
			int L = ((Per_Size << 1) + 9 - size) >> 1, l = L;
			for(int i = t -> head; i <= t -> tail; i++) t -> data[L++] = t -> data[i];
			for(int i = p -> head; i <= p -> tail; i++) t -> data[L++] = p -> data[i];
			t -> head = l; t -> tail = L - 1;
		}
		else{
			for(int i = p -> head; i <= p -> tail; i++) t -> data[++(t -> tail)] = p -> data[i];
		}
		t -> next = p -> next; 
		if(p -> next != NULL) p -> next -> pre = t;
		delete [] p -> data;
		delete p;
	}
	
	void Split(List *t, const int pos) { // �� t �ӵ�pos��Ԫ�غ����п�
		int L = t -> head + pos;
		List *p = new List(Per_Size);
		p -> head = 1; p -> tail = 0;
		for(int i = L; i <= t -> tail; i++) p -> data[++(p -> tail)] = t -> data[i];
		t -> tail = L - 1;
		p -> next = t -> next; p -> pre = t; t -> next = p;
		if(p -> next != NULL) p -> next -> pre = p;
	} 
	
	T* Find(int pos) const {
		List *p = first;
		int w = pos;
		while(p -> Get_Size() < w){
			w -= p -> Get_Size();
			p = p -> next;
		}
		return p -> data[p -> head + w - 1];
	}
	
	void Clear(){
		List *p = first, *t;
		while(p != NULL){
			for(int i = p -> head; i <= p -> tail; i++) delete (p -> data[i]);
			t = p -> next;
			delete [] p -> data;
			delete p;
			p = t;
		}
	}
	
public:
	class const_iterator;
	class iterator {
	private:
		int pos;
		List *W;
		deque *root;
	
		inline void plus(int step){
			if(!step) return;
			while(step > (W -> tail - pos)){
				step -= W -> tail - pos + 1;
				W = W -> next;
				if(W == NULL){
					pos = 0;return;
				}
				pos = W -> head;
			}
			pos += step;
		}
		
		inline void dec(int step){
			if(!step) return;
			if(W == NULL){
				step--; W = root -> last; pos = W -> tail;
			}
			while(step > (pos - W -> head)){
				step -= pos - W -> head + 1;
				W = W -> pre;
				if(W == NULL) return;
				pos = W -> tail;
			}
			pos -= step;
		}
		
	public:
		
		int Get_pos() const {return pos;}
		List* Get_W() const {return W;}
		deque* Get_root() const {return root;}
		
		iterator() : pos(0), W(NULL), root(NULL) {}
		iterator(const int &Pos, List *_W, deque* Root) : pos(Pos), W(_W), root(Root) {}
		iterator(const iterator &rhs){
			pos = rhs.pos; W = rhs.W; root = rhs.root;
		}

		iterator operator+(const int &n) const {
			iterator it(pos, W, root);
			if(n >= 0) it.plus(n);
			else it.dec(-n);
			return it;
		}
		iterator operator-(const int &n) const {
			iterator it(pos, W, root);
			if(n >= 0) it.dec(n);
			else it.plus(-n);
			return it;
		}
		
		int operator-(const iterator &rhs) const {
			if(root != rhs.root) throw invalid_iterator();
			int Rpos = rhs.pos; List* RW = rhs.W;
			if(RW == NULL) RW = root -> last, Rpos = RW -> tail + 1;
			int res = 0, npos = pos; List* nW = W;
			if(nW == NULL) nW = root -> last, npos = nW -> tail + 1;
			while(nW != NULL && nW != RW){
				res -= nW -> tail - npos + 1;
				nW = nW -> next; 
				if(nW != NULL) npos = nW -> head;
			}
			if(nW != NULL){
				return res + npos - Rpos;
			}
			else{
				res = 0; npos = Rpos; List* nW = RW;
				List *pW = W; int ppos = pos;
				if(pW == NULL) pW = root -> last, ppos = pW -> tail + 1;
				while(nW != NULL && nW != pW){
					res += nW -> tail - npos + 1;
					nW = nW -> next; 
					npos = nW -> head;
				}
				return res + ppos - npos;
			}
		}
		iterator operator+=(const int &n) {
			return *this = *this + n;
		}
		iterator operator-=(const int &n) {
			return *this = *this - n;
		}

		iterator operator++(int) {
			if(W == NULL) throw invalid_iterator();
			int npos = pos; List *nW = W;
			plus(1);
			return iterator(npos, nW, root);
		}

		iterator& operator++() {
			if(W == NULL) throw invalid_iterator();
			plus(1);
			return *this;
		}

		iterator operator--(int) {
			if(!root -> sum) throw invalid_iterator();
			if(W == root -> first && pos == W -> head) throw invalid_iterator();
			int npos = pos; List *nW = W;
			dec(1);
			return iterator(npos, nW, root);
		}

		iterator& operator--() {
			if(!root -> sum) throw invalid_iterator();
			if(W == root -> first && pos == W -> head) throw invalid_iterator();
			dec(1);
			return *this;
		}

		T& operator*() const {
			if(W == NULL) throw invalid_iterator();
			return *(W -> data[pos]);
		}
		
		T* operator->() const noexcept {
			if(W == NULL) throw invalid_iterator();
			return (W -> data[pos]);
		}

		bool operator==(const iterator &rhs) const {
			return (pos == rhs.pos && W == rhs.W && root == rhs.root);
		}
		
		bool operator==(const const_iterator &rhs) const {
			return (pos == rhs.Get_pos() && W == rhs.Get_W() && root == rhs.Get_root());
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
		int pos;
		List *W;
		const deque *root;
		
		inline void plus(int step){
			if(!step) return;
			while(step > (W -> tail - pos)){
				step -= W -> tail - pos + 1;
				W = W -> next;
				if(W == NULL){
					pos = 0;return;
				}
				pos = W -> head;
			}
			pos += step;
		}
		
		inline void dec(int step){
			if(!step) return;
			if(W == NULL){
				step--; W = root -> last; pos = W -> tail;
			}
			while(step > (pos - W -> head)){
				step -= pos - W -> head + 1;
				W = W -> pre;
				if(W == NULL) return;
				pos = W -> tail;
			}
			pos -= step;
		}
		
	public:
		
		int Get_pos() const {return pos;}
		List* Get_W() const {return W;}
		const deque* Get_root() const {return root;}
		
		const_iterator() : pos(0), W(NULL), root(NULL) {}
		const_iterator(const int &Pos, List* _W, const deque* Root) : pos(Pos), W(_W), root(Root) {}
		const_iterator(const iterator &rhs){
			pos = rhs.Get_pos(); W = rhs.Get_W(); root = rhs.Get_root();
		}
		const_iterator(const const_iterator &rhs){
			pos = rhs.pos; W = rhs.W; root = rhs.root;
		}

		const_iterator operator+(const int &n) const {
			const_iterator it(pos, W, root);
			if(n >= 0) it.plus(n);
			else it.dec(-n);
			return it;
		}
		const_iterator operator-(const int &n) const {
			const_iterator it(pos, W, root);
			if(n >= 0) it.dec(n);
			else it.plus(-n);
			return it;
		}
		
		int operator-(const const_iterator &rhs) const {
			if(root != rhs.root) throw invalid_iterator();
			int Rpos = rhs.pos; List* RW = rhs.W;
			if(RW == NULL) RW = root -> last, Rpos = RW -> tail + 1;
			int res = 0, npos = pos; List* nW = W;
			if(nW == NULL) nW = root ->last, npos = nW -> tail + 1;
			while(nW != NULL && nW != RW){
				res -= nW -> tail - npos + 1;
				nW = nW -> next; 
				if(nW != NULL) npos = nW -> head;
			}
			if(nW != NULL){
				return res + npos - Rpos;
			}
			else{
				res = 0; npos = Rpos; List* nW= RW;
				List *pW = W; int ppos = pos;
				if(pW == NULL) pW = root -> last, ppos = pW -> tail + 1;
				while(nW != NULL && nW != pW){
					res += nW -> tail - npos + 1;
					nW = nW -> next; 
					npos = nW -> head;
				}
				return res + ppos - npos;
			}
		}
		
		const_iterator operator+=(const int &n) {
			return *this = *this + n;
		}
		
		const_iterator operator-=(const int &n) {
			return *this = *this - n;
		}

		const_iterator operator++(int) {
			if(W == NULL) throw invalid_iterator();
			int npos = pos; List *nW = W;
			plus(1);
			return const_iterator(npos, nW, root);
		}

		const_iterator& operator++() {
			if(W == NULL) throw invalid_iterator();
			plus(1);
			return *this;
		}

		const_iterator operator--(int) {
			if(!root -> sum) throw invalid_iterator();
			if(W == root -> first && pos == W -> head) throw invalid_iterator();
			int npos = pos; List *nW = W;
			dec(1);
			return const_iterator(npos, nW, root);
		}
	
		const_iterator& operator--() {
			if(!root -> sum) throw invalid_iterator();
			if(W == root -> first && pos == W -> head) throw invalid_iterator();
			dec(1);
			return *this;
		}

		const T& operator*() const {
			if(W == NULL) throw invalid_iterator();
			return *(W -> data[pos]);
		}
		
		T const * operator->() const noexcept {
			if(W == NULL) throw invalid_iterator();
			return (W -> data[pos]);
		}

		bool operator==(const iterator &rhs) const {
			return (pos == rhs.Get_pos() && root == rhs().Get_root);
		}
		
		bool operator==(const const_iterator &rhs) const {
			return (pos == rhs.pos && root == rhs.root);
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

	deque() : sum(0), Per_Size(3), Sqrt(0) {
		first = new List(Per_Size);
		last = first;
	}
	deque(const deque &other) {
		Per_Size = other.Per_Size; Sqrt = other.Sqrt;
		List *p, *t = other.first, *Pre;
		first = new List(Per_Size); p = first;
		p -> head = t -> head; p -> tail = t -> tail;
		for(int i = t -> head; i <= t -> tail; i++) p -> data[i] = new T(*(t -> data[i]));
		Pre = first; t = t -> next;
		while(t != NULL){
			p = new List(Per_Size);
			p -> head = t -> head; p -> tail = t -> tail;
			p -> pre = Pre; 
			Pre -> next = p;
			for(int i = t -> head; i <= t -> tail; i++) p -> data[i] = new T(*(t -> data[i]));
			Pre = p; t = t -> next;
		}
		last = Pre; sum = other.sum; 
	}

	~deque() {
		if(first != NULL) Clear();
	}

	deque &operator=(const deque &other) {
		if(this == &other) return *this;
		if(first != NULL) Clear();
		Per_Size = other.Per_Size; Sqrt = other.Sqrt;
		List *p, *t = other.first, *Pre;
		first = new List(Per_Size); p = first;
		p -> head = t -> head; p -> tail = t -> tail;
		for(int i = t -> head; i <= t -> tail; i++) p -> data[i] = new T(*(t -> data[i]));
		Pre = first; t = t -> next;
		while(t != NULL){
			p = new List(Per_Size);
			p -> head = t -> head; p -> tail = t -> tail;
			p -> pre = Pre; 
			Pre -> next = p;
			for(int i = t -> head; i <= t -> tail; i++) p -> data[i] = new T(*(t -> data[i]));
			Pre = p; t = t -> next;
		}
		last = Pre; sum = other.sum; 
		return *this;
	}

	T & at(const size_t &pos) {
		int Pos = pos + 1;
		if(Pos < 1 || Pos > sum) throw index_out_of_bound();
		return *(Find(Pos));
	}
	
	const T & at(const size_t &pos) const {
		int Pos = pos + 1;
		if(Pos < 1 || Pos > sum) throw index_out_of_bound();
		return *(Find(Pos));
	}
	
	T & operator[](const size_t &pos) {
		int Pos = pos + 1;
		if(Pos < 1 || Pos > sum) throw index_out_of_bound();
		return *(Find(Pos));
	}
	
	const T & operator[](const size_t &pos) const {
		int Pos = pos + 1;
		if(Pos < 1 || Pos > sum) throw index_out_of_bound();
		return *(Find(Pos));
	}

	const T & front() const {
		if(sum == 0) throw container_is_empty();
		return *(first -> data[first -> head]);
	}

	const T & back() const {
		if(sum == 0) throw container_is_empty();
		return *(last -> data[last -> tail]);
	}

	iterator begin() {
		if(!sum) return iterator(0, NULL, this);
		return iterator(first -> head, first, this);
	}
	
	const_iterator cbegin() const {
		if(!sum) return const_iterator(0, NULL, this);
		return const_iterator(first -> head, first, this);
	}

	iterator end() {
		return iterator(0, NULL, this);
	}
	
	const_iterator cend() const {
		return const_iterator(0, NULL, this);
	}

	bool empty() const {
		return sum == 0;
	}

	size_t size() const {
		return sum;
	}

	void clear() {
		sum = 0; Sqrt = 0; Per_Size = 3;
		if(first != NULL) Clear();
		first = new List(Per_Size);
		last = first;
	}

	iterator insert(iterator pos, const T &value) {
		if(pos.Get_root() != this) throw invalid_iterator();
		int W = pos.Get_pos(), Pos = 0;
		List *p = pos.Get_W();
		if(p == NULL) p = last, W = last -> tail + 1;
		List *now = first;
		while(now != p){
			Pos += now -> tail - now -> head + 1;
			now = now -> next;
		}
		Pos += W - p -> head + 1;
		if(p -> head > 1){
			for(int i = (p -> head - 1); i < W - 1; i++) p -> data[i] = p -> data[i + 1];
			--(p -> head);
			p -> data[W - 1] = new T(value);
		}
		else{
			for(int i = (p -> tail + 1); i > W; i--) p -> data[i] = p -> data[i - 1];
			++(p -> tail);
			p -> data[W] = new T(value);
		}
		if(p -> Get_Size() >= (Per_Size >> 1) + Per_Size){
			Split(p, (p -> Get_Size()) >> 1);
			if(last == p) last = p -> next;
		}
		Add_sum();
		now = first;
		while(Pos > now -> Get_Size()){
			Pos -= now -> tail - now -> head + 1;
			now = now -> next;
		}
		Pos = now -> head + Pos - 1;
		return iterator(Pos, now, this);
	}

	iterator erase(iterator pos) {
		if(pos.Get_root() != this) throw invalid_iterator();
		if(pos.Get_W() == NULL || pos.Get_pos() == 0) throw invalid_iterator();
		int W = pos.Get_pos(), Pos = 0;
		List *p = pos.Get_W();
		List *now = first;
		while(now != p){
			Pos += now -> tail - now -> head + 1;
			now = now -> next;
		}
		Pos += W - p -> head + 1;
		delete p -> data[W];
		for(int i = W; i < p -> tail; i++) p -> data[i] = p -> data[i + 1];
		--(p -> tail);
		if(p -> Get_Size() < (Per_Size >> 1)){
			if(p -> next != NULL){
				if(last == p -> next) last = p;
				Merge(p, p -> next);
				if(p -> Get_Size() >= (Per_Size >> 1) + Per_Size){
					Split(p, (p -> Get_Size()) >> 1);
					if(last == p) last = p -> next;
				}
			} else
			if(p -> pre != NULL){
				List *t = p -> pre;
				if(last == p) last = t;
				Merge(t, t -> next);
				if(t -> Get_Size() >= (Per_Size >> 1) + Per_Size){
					Split(t, (t -> Get_Size()) >> 1);
					if(last == t) last = t -> next;
				}
				p = t;
			}
		}
		Dec_sum();
		if(Pos > sum) return iterator(0, NULL, this);
		now = first;
		while(Pos > now -> Get_Size()){
			Pos -= now -> tail - now -> head + 1;
			now = now -> next;
		}
		Pos = now -> head + Pos - 1;
		return iterator(Pos, now, this);
	}

	void push_back(const T &value) {
		if(last -> tail  + 1 >= (Per_Size << 1) + 6) Balance(last);
		last -> data[++(last -> tail)] = new T(value);
		if(last -> Get_Size() >= (Per_Size >> 1) + Per_Size){
			Split(last, (last -> Get_Size()) >> 1);
			last = last -> next;
		}
		Add_sum();
	}

	void pop_back() {
		if(sum == 0) throw container_is_empty();
		delete last -> data[(last -> tail)--];
		if(last -> Get_Size() < (Per_Size >> 1)){
			if(last -> pre != NULL){
				last = last -> pre;
				Merge(last, last -> next);
				if(last -> Get_Size() >= (Per_Size >> 1) + Per_Size){
					Split(last, (last -> Get_Size()) >> 1);
					last = last -> next;
				}
			}
		}
		Dec_sum();
	}

	void push_front(const T &value) {
		if(first -> head <= 1) Balance(first);
		first -> data[--(first -> head)] = new T(value);
		if(first -> Get_Size() >= (Per_Size >> 1) + Per_Size){
			Split(first, (first -> Get_Size()) >> 1);
			if(last == first) last = first -> next;
		}
		Add_sum();
	}

	void pop_front() {
		if(sum == 0) throw container_is_empty();
		delete first -> data[(first -> head)++];
		if(first -> Get_Size() < (Per_Size >> 1)){
			if(first -> next != NULL){
				if(last == first -> next) last = first; 
				Merge(first, first -> next);
				if(first -> Get_Size() >= (Per_Size >> 1) + Per_Size){
					Split(first, (first -> Get_Size()) >> 1);
					if(last == first) last = first -> next;
				}
			}
		}
		Dec_sum();
	}
};

}

#endif
